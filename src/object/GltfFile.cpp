//
// Created by xxw on 2023/2/16.
//


#include <fstream>
#include <iostream>

#include <filesystem.hpp>
#include <json/json.h>
#include <lodepng_util.h>

#include "GltfFile.h"
#include "ObjectOperator.h"

static bool icasecompare(const std::string& a, const std::string& b) {
    if (a.length() == b.length()) {
        return std::equal(a.begin(), a.end(), b.begin(),
                          [](char a, char b) {
                              return tolower(a) == tolower(b);
                          });
    }
    else {
        return false;
    }
}


GltfFile::GltfFile(const std::string& filename):GltfFile() {
    load(filename);
}


GltfFile::GltfFile(std::ifstream file): GltfFile() {
    loadGlb(file);
}

bool GltfFile::load(const std::string& filename) {

    ghc::filesystem::path path(filename);
    //判断文件存不存在
    if(!exists(path)) {
        std::cout << "file not found: " << filename << std::endl;
        return false;
    }
    auto pathdir=path;
    pathdir = pathdir.remove_filename();
    this->filePath=pathdir.string();
    std::ifstream file;
    file.open(filename,std::ios_base::in|std::ios_base::binary);
    if(!file.is_open()){
        std::cout << "file not found: " << filename << std::endl;
        return false;
    }
    if(path.has_extension()&& icasecompare(path.extension().string(),".gltf")){
        return loadGltf(file);
    }
    return loadGlb(file);
}

bool GltfFile::loadGlb(std::ifstream &file) {
    if(!file.is_open()) return false;

    char headerMagic[5]={0};
    int headerVersion;
    int length;

    file.seekg(0,std::ios::end);
    std::ios::pos_type fileLength = file.tellg();
    file.seekg(0,std::ios::beg);

    if(fileLength<12){
        file.close();
        return false;
    }

    //检测magic 是否为GLTF
    file.read(headerMagic,4);
    if(std::string(headerMagic)!="glTF"){
        file.close();
        return false;
    }

    file.read((char*)&headerVersion,4);
    file.read((char*)&length,4);
    if(length>fileLength){
        file.close();
        return false;
    }

    //读取JSON chunk
    int jsonLength=0;
    char jsonType[5]={0};
    file.read((char*)&jsonLength,4);
    file.read((char*)&jsonType,4);
    if(*(int*)&jsonType!=0x4E4F534A){
        file.close();
        return false;
    }

    if(jsonLength<=0 || jsonLength>(length-20)){
        file.close();
        return false;
    }

    //读取JSON数据部分
    char* jsonData=new char[jsonLength];
    file.read(jsonData,jsonLength);
    auto *jsonRoot=new Json::Value();
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    JSONCPP_STRING errs;
    if (!reader->parse(jsonData, jsonData+jsonLength, jsonRoot,
                       &errs)) {
        delete[] jsonData;
        file.close();
        return false;
    }
    delete[] jsonData;



    //读取bin
    int binLen=0;
    char binType[5]={0};
    byte *binData = nullptr;
    file.read((char*)&binLen,4);
    file.read((char*)&binType,4);
    if(*(int*)&binType==0x4E4942){
        binData=new byte[binLen];
        file.read((char*)binData,binLen);
    }
    //关闭文件
    file.close();

    this->json=jsonRoot;
    this->bin=binData;
    this->binLength=binLen;
    active= true;

    return true;
}

void GltfFile::close() {
    if(active){
        delete (Json::Value*)json;
        delete (byte*)bin;
        json=nullptr;
        bin=nullptr;
        binLength=0;
        active= false;
        for(auto i:this->uriData){
            delete i.second.data;
        }
        delete jsonBuffers;
    }
}

Object3D *GltfFile::newMesh() {

    auto *root=(Json::Value*)this->json;
    if(root==nullptr){
        return nullptr;
    }
    Json::Value accessors;
    Json::Value bufferViews;
    Json::Value buffers;
    Json::Value images;
    Json::Value materials;
    Json::Value meshes;
    Json::Value nodes;
    Json::Value samplers;
    Json::Value scenes;
    Json::Value textures;

    //解析第0个scenes
    Json::Value node;
    Json::Value mesh;

    auto *object=new Object3D;

    try{
        accessors=root->operator[]("accessors");
        bufferViews=root->operator[]("bufferViews");
        buffers=root->operator[]("buffers");
        images=root->operator[](("images"));
        materials=root->operator[](("materials"));
        meshes =root->operator[]("meshes");
        nodes=root->operator[]("nodes");
        samplers=root->operator[]("samplers");
        scenes=root->operator[]("scenes");
        textures=root->operator[]("textures");
        //默认解析第0个scenes
        int nodeIndex=scenes[0]["nodes"][0].asInt();
        //获取根节点
        node=nodes[nodeIndex];
    }catch (std::exception &e){
        std::cout<<e.what()<<std::endl;
        delete object;
        return nullptr;
    }
    this->jsonBuffers=new Json::Value;
    *this->jsonBuffers=buffers;


    //默认解析第0个mesh
    if(!node["mesh"].empty()){
        mesh=meshes[node["mesh"].asInt()];
    }else{
        //遍历所有children节点直至找到mesh
        Json::Value children=node["children"];
        for(const auto& child:nodes){
            if(!child["mesh"].empty()){
                mesh=meshes[child["mesh"].asInt()];
                break;
            }
        }
    }

    if(mesh.empty()){
        delete object;
        return nullptr;
    }


    int faceIndex=mesh["primitives"][0][ "indices"].asInt();
    int vertexIndex=mesh["primitives"][0]["attributes"]["POSITION"].asInt();

    Json::Value faceAccessor=root->operator[]("accessors")[faceIndex];
    Json::Value vertexAccessor=root->operator[]("accessors")[vertexIndex];

    int faceType=faceAccessor["componentType"].asInt();
    if(faceType!=5123/*unsigned short*/){
        delete object;
        return nullptr;
    }

    //加载面数据
    Json::Value faceBufferView=bufferViews[faceAccessor["bufferView"].asInt()];
    {
        int buffer=faceBufferView["buffer"].asInt();
        int byteOffset=faceBufferView["byteOffset"].asInt();
        int byteLength=faceBufferView["byteLength"].asInt();
        int target=faceBufferView["target"].asInt();
        //target 34963 ELEMENT_ARRAY_BUFFER
        unsigned int length=0;
        const byte* bin=this->getBuffer(buffer,&length);

        byte *bufferAddress=&(((byte*)bin)[byteOffset]);

        object->faceLength=faceAccessor["count"].asInt()/3;
        object->face=new Object3D::Face[object->faceLength];
        auto *face=( unsigned short *)bufferAddress;
        for(int i=0;i<object->faceLength;i++){
            object->face[i].a=face[i*3];
            object->face[i].b=face[i*3+1];
            object->face[i].c=face[i*3+2];
        }
    }

    //加载顶点数据
    Json::Value vertexBufferView=bufferViews[vertexAccessor["bufferView"].asInt()];
    {
        int buffer=vertexBufferView["buffer"].asInt();
        int byteOffset=vertexBufferView["byteOffset"].asInt();
        int byteLength=vertexBufferView["byteLength"].asInt();
        int target=vertexBufferView["target"].asInt();
        unsigned int length=0;
        const byte* bin=this->getBuffer(buffer,&length);
        byte *bufferAddress=&(((byte*)bin)[byteOffset]);
        delete object->vertex;
        object->vertexLength=vertexAccessor["count"].asInt();
        object->vertex=new Vector4f[object->vertexLength];
        object->viewVertex=new Vector4f[object->vertexLength];
        object->normalVector=new Vector3f[object->vertexLength];

        auto* vertex=(float *)bufferAddress;
        for(int i=0;i<object->vertexLength;i++){
            object->vertex[i].x=vertex[i*3+0];
            object->vertex[i].y=vertex[i*3+1];
            object->vertex[i].z=vertex[i*3+2];
            object->vertex[i].w=1;
        }

    }

    ObjectOperator opt(object);
    opt.calculateNormalVector();//自动计算法向量
    opt.calculateCenterAndSize();
    delete object->attributes;
    object->attributes=new Object3D::Attributes[object->vertexLength];


    //加载 texcoord 决定如何映射贴图
    Json::Value texcoordIndex= mesh["primitives"][0]["attributes"]["TEXCOORD_0"];
    if(!texcoordIndex.empty()){
        Json::Value texcoordAccessor = root->operator[]("accessors")[texcoordIndex.asInt()];
        if(!texcoordAccessor.empty()){
            Json::Value bufferViewIndex=texcoordAccessor["bufferView"];
            if(bufferViewIndex.empty()){
                printf("texcoord bufferView null");
            }
            if(texcoordAccessor["componentType"].asInt()!=5126){
                printf("texcoord componentType is no 5126");
            }
            unsigned int count=texcoordAccessor["count"].asInt();
            if(count!=object->vertexLength){
                printf("texcoord count error");
            }

            if(texcoordAccessor["type"].asString()!="VEC2"){
                printf("texcoord type is not VEC2");
            }

            Json::Value bufferView=bufferViews[bufferViewIndex.asInt()];
            int buffer=bufferView["buffer"].asInt();
            int byteOffset=bufferView["byteOffset"].asInt();
            int byteLength=bufferView["byteLength"].asInt();
            unsigned int length=0;
            const byte* bin=this->getBuffer(buffer,&length);
            byte *bufferAddress=&(((byte*)bin)[byteOffset]);
            if (byteLength < (count*2*4)){
                printf("texcoord buffer length error");
            }
            auto *texcoord=(float*)bufferAddress;
            for(int i=0;i<object->vertexLength;i++){
                object->attributes[i].textureCoord.x=texcoord[2*i];
                object->attributes[i].textureCoord.y=texcoord[2*i+1];
            }
        }
    }

    //加载材质
    Json::Value materialIndex=mesh["primitives"][0]["material"];
    Json::Value material = root->operator[]("materials")[materialIndex.asInt()];

    //加载diffuseTexture
    Json::Value diffuseBufferView;
    Json::Value diffuseTextureIndex=material["extensions"]["KHR_materials_pbrSpecularGlossiness"]["diffuseTexture"]["index"];
    if(!diffuseTextureIndex.empty()){
        Json::Value source=root->operator[]("textures")[diffuseTextureIndex.asInt()]["source"];
        if(!source.empty()){
             if(images[source.asInt()]["mimeType"].asString()=="image/png"){
                 Json::Value bufferViewIndex=images[source.asInt()]["bufferView"];
                 if(!bufferViewIndex.empty()){
                     diffuseBufferView=bufferViews[bufferViewIndex.asInt()];
                 }
             }
        }
    }
    if(!diffuseBufferView.empty()){
        int buffer=diffuseBufferView["buffer"].asInt();
        int byteOffset=diffuseBufferView["byteOffset"].asInt();
        int byteLength=diffuseBufferView["byteLength"].asInt();
        unsigned int length=0;
        const byte* bin=this->getBuffer(buffer,&length);
        byte *bufferAddress=&(((byte*)bin)[byteOffset]);

        auto *in=new std::vector<unsigned char >;
        auto *png=new std::vector<unsigned char >;
        in->resize(byteLength);
        memmove(in->data(),bufferAddress,byteLength);
        unsigned int w=0,h=0;
        lodepng::State state;
        lodepng::decode(*png,w,h,*in);
        object->diffuseTextureWidth=w;
        object->diffuseTextureHeight=h;
        delete object->diffuseTexture;
        object->diffuseTexture=new RGBAF[w*h];
        for(int i=0;i<h;i++){
            for(int j=0;j<w;j++){
                RGBAF *rgb=&(object->diffuseTexture[(i*w+j)]);
                rgb->a=1.0;
                rgb->r=(float)((*png)[4*(i*w+j)+0])/255;
                rgb->g=(float)((*png)[4*(i*w+j)+1])/255;
                rgb->b=(float)((*png)[4*(i*w+j)+2])/255;
            }
        }
        delete png;
        delete in;

    }



    return object;

}

GltfFile::GltfFile() {
    jsonBuffers=nullptr;
    bin=nullptr;
    json= nullptr;
    active=false;
}

bool GltfFile::loadGltf(std::ifstream &file) {
    //读取JSON数据部分
    file.seekg(0,std::ios::end);
    auto jsonLength=file.tellg();
    file.seekg(0,std::ios::beg);
    char* jsonData=new char[jsonLength];
    file.read(jsonData,jsonLength);
    auto *jsonRoot=new Json::Value();
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    JSONCPP_STRING errs;
    if (!reader->parse(jsonData, jsonData+jsonLength, jsonRoot,
                       &errs)) {
        delete[] jsonData;
        file.close();
        return false;
    }
    this->json=jsonRoot;
    delete[] jsonData;
    this->active=true;
}

bool GltfFile::loadUriResource(const std::string& uri) {
    auto path=ghc::filesystem::path(this->filePath);
    path.append(uri);
    std::ifstream f;
    f.open(path.string(),std::ios::in|std::ios::binary);
    if(f.is_open()){
        f.seekg(0,std::ios::end);
        auto len= f.tellg();
        f.seekg(0,std::ios::beg);
        auto data=new byte[len];
        f.read((char*)data,len);
        f.close();
        UriData d;
        d.data=(unsigned char*)data;
        d.length=len;
        d.name=uri;
        this->uriData[uri]=d;

    }else{
        return false;
    }
}

const byte *GltfFile::getBuffer(unsigned int index, unsigned int *length) {
    if(this->jsonBuffers){
        Json::Value buffer=this->jsonBuffers->operator[](index);
        Json::Value uri=buffer["uri"];
        if(uri.empty()){
            *length=this->binLength;
            return this->bin;
        }else{

            if(this->uriData.find(uri.asString())==this->uriData.end()){
                loadUriResource(uri.asString());
            }
            if(this->uriData.find(uri.asString())==this->uriData.end()){
                return nullptr;
            }
            *length =  this->uriData[uri.asString()].length;
            return this->uriData[uri.asString()].data;
        }
    }
}
