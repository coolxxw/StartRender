//
// Created by xxw on 2023/2/16.
//


#include <fstream>
#include <iostream>

#include "filesystem.hpp"
#include "json/json.h"
#include "lodepng_util.h"
#include "turbojpeg.h"

#include "../include/GltfFile.h"
#include "../math/Matrix.h"
#include "ImageTool.h"

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
        for(const auto& i:this->uriData){
            delete i.second.data;
        }
        for(auto i:this->tempData){
            delete[] (char*)i;
        }
        delete jsonBuffers;
    }
}


GltfFile::GltfFile() {
    jsonBuffers=nullptr;
    bin=nullptr;
    json= nullptr;
    active=false;
    binLength=0;
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
    return true;
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

    return true;
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

bool GltfFile::fillScene(render::Scene* sceneResource) {
    auto *root=(Json::Value*)this->json;
    if(root==nullptr){
        return false;
    }
    Json::Value images;
    Json::Value materials;
    Json::Value meshes;
    Json::Value nodes;
    Json::Value scenes;
    Json::Value textures;

    //解析第0个scenes
    Json::Value mesh;
    Json::Value material;
    int nodeIndex=0;
    try{
        images=root->operator[](("images"));
        materials=root->operator[](("materials"));
        meshes =root->operator[]("meshes");
        nodes=root->operator[]("nodes");
        scenes=root->operator[]("scenes");
        textures=root->operator[]("textures");
        //默认解析第0个scenes
        nodeIndex=scenes[0]["nodes"][0].asInt();

    }catch (std::exception &e){
        std::cout<<e.what()<<std::endl;
        return false;
    }



    //默认解析第0个node第0个节点
    std::stack<int> stack;
    std::stack<Matrix4f> stack2;
    stack.push(nodeIndex);
    stack2.push(Matrix4f());

    //递归查找mesh 使用stack替代递归
    while(!stack.empty()){
        int node_i=stack.top();
        stack.pop();
        auto matrix=stack2.top();
        stack2.pop();
        auto node=nodes[node_i];
        if(node.empty()){
            continue;
        }
        if (!node["translation"].empty()) {
            auto trans = node["translation"];
            matrix.move(Vector3f(trans[0].asFloat(), trans[1].asFloat(), trans[2].asFloat()));
        }
        if (!node["scale"].empty()) {
            auto scale = node["scale"];
            matrix.scale(Vector3f(scale[0].asFloat(), scale[1].asFloat(), scale[2].asFloat()));
        }
        if (!node["rotation"].empty()) {
            auto rotation = node["rotation"];
            matrix.rotate(Vector3f(rotation[0].asFloat(), rotation[1].asFloat(), rotation[2].asFloat()));
        }
        if(!node["matrix"].empty()){
            auto mn=node["matrix"];
            auto matrix2= Matrix4f{mn[0].asFloat(),mn[4].asFloat(),mn[8].asFloat(),mn[12].asFloat(),
                            mn[1].asFloat(),mn[5].asFloat(),mn[9].asFloat(),mn[13].asFloat(),
                            mn[2].asFloat(),mn[6].asFloat(),mn[10].asFloat(),mn[14].asFloat(),
                            mn[3].asFloat(),mn[7].asFloat(),mn[11].asFloat(),mn[15].asFloat()};
            matrix=matrix*matrix2;
        }
        if(!node["mesh"].empty()) {
            auto meshId = addMesh(sceneResource, node["mesh"].asInt());
            sceneResource->translationMesh(meshId,matrix);
        }

        if(node["children"]){
            for(auto i:node["children"]){
                stack.push(i.asInt());
                stack2.push(matrix);
            }
        }

    }
    return true;
}

const byte *GltfFile::getUriData(const std::string& uri_file, unsigned int *dataLength) {
    if(this->uriData.find(uri_file) != this->uriData.end()){
        *dataLength = this->uriData[uri_file].length;
        return this->uriData[uri_file].data;
    }
    auto path=ghc::filesystem::path(this->filePath);
    path.append(uri_file);
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
        d.name=uri_file;
        this->uriData[uri_file]=d;
        *dataLength=len;
        return data;
    }

    return nullptr;
}

const byte *GltfFile::getIndices(unsigned int index, unsigned int *dataLength) {
    if (json->empty()) {
        return nullptr;
    }
    auto accessors=json->operator[]("accessors");
    if(accessors.empty()){
        return nullptr;
    }
    auto position=accessors[index];
    if(position.empty()){
        return nullptr;
    }
    auto bufferView=position["bufferView"];
    auto byteOffset=position["byteOffset"];
    auto componentType=position["componentType"];
    auto count=position["count"];
    auto type=position["type"];
    //UNSIGNED INT或UNSIGNED SHORT
    if(componentType.asInt()!=5123 && componentType.asInt()!=5125){
        return nullptr;
    }
    if(type.asString()!="SCALAR"){
        return nullptr;
    }
    unsigned int length=0;
    auto d=this->getBufferView(bufferView.asInt(),&length);

    //unsigned short
    if(componentType.asInt()==5123){
        //检测长度 数量*标量1*UNSIGNED SHORT长度2
        if((length+byteOffset.asInt())<(count.asInt()*2)){
            return nullptr;
        }
        int count_int=count.asInt();
        auto data=new unsigned int[count_int];
        d=&(d[byteOffset.asInt()]);
        for(int i=0;i<count_int;i++){
            data[i]=((unsigned short *)d)[i];
        }
        this->tempData.push_back((void*)data);
        *dataLength=count_int*4;
        return (byte*)data;
    }else if(componentType.asInt()==5125){//unsigned int
        //检测长度 数量*标量1*UNSIGNED INT长度4
        if(length<(count.asInt()*4)){
            return nullptr;
        }
        *dataLength=length;
        return d+byteOffset.asInt();
    }
    return nullptr;

}

const byte *GltfFile::getPosition(unsigned int index, unsigned int *dataLength) {
    if (json->empty()) {
        return nullptr;
    }
    auto accessors=json->operator[]("accessors");
    if(accessors.empty()){
        return nullptr;
    }
    auto position=accessors[index];
    if(position.empty()){
        return nullptr;
    }
    auto bufferView=position["bufferView"];
    auto componentType=position["componentType"];
    auto count=position["count"];
    auto type=position["type"];
    auto byteOffset=position["byteOffset"].asInt();
    //FLOAT
    if(componentType.asInt()!=5126){
        return nullptr;
    }
    if(type.asString()!="VEC3"){
        return nullptr;
    }
    unsigned int length=0;
    auto d=this->getBufferView(bufferView.asInt(),&length);
    //检测长度 数量*向量长度3*FLOAT长度4
    if( (length-byteOffset)<(count.asInt()*3*4)){
        return nullptr;
    }
    *dataLength=(count.asInt()*3*4);
    return d+byteOffset;
}

const byte *GltfFile::getTexCoord(unsigned int index, unsigned int *dataLength) {
    if (json->empty()) {
        return nullptr;
    }
    auto accessors=json->operator[]("accessors");
    if(accessors.empty()){
        return nullptr;
    }
    auto position=accessors[index];
    if(position.empty()){
        return nullptr;
    }
    auto bufferView=position["bufferView"];
    auto componentType=position["componentType"];
    auto count=position["count"];
    auto type=position["type"];
    auto byteOffset=position["byteOffset"].asInt();
    //FLOAT
    if(componentType.asInt()!=5126){
        return nullptr;
    }
    if(type.asString()!="VEC2"){
        return nullptr;
    }
    unsigned int length=0;
    auto d=this->getBufferView(bufferView.asInt(),&length);
    //检测长度 数量*向量长度2*FLOAT长度4
    if((length-byteOffset)<(count.asInt()*2*4)){
        return nullptr;
    }
    *dataLength=(count.asInt()*2*4);
    return d+byteOffset;
}

//返回Image原始数据
const byte *GltfFile::getImage(unsigned int index, std::string &name, unsigned int *dataLength) {
    if (json->empty()) {
        return nullptr;
    }
    auto images=json->operator[]("images");
    if(images.empty()){
        return nullptr;
    }
    auto image=images[index];
    if(image.empty()){
        return nullptr;
    }
    auto uri=image["uri"];
    if(!uri.empty()){
        unsigned int length;
        auto data=this->getUriData(uri.asString(),&length);
        name=uri.asString();
        *dataLength=length;
        return data;
    }
    return nullptr;

}

const byte *GltfFile::getTextureRGBA(unsigned int index, std::string &name,unsigned int *w, unsigned int *h) {
    if (json->empty()) {
        return nullptr;
    }
    auto textures=json->operator[]("textures");
    if(textures.empty()){
        return nullptr;
    }
    auto texture=textures[index];
    if(texture.empty()){
        return nullptr;
    }
    auto source=texture["source"];
    if(source.empty()){
        return nullptr;
    }
    unsigned int dataLength;
    auto imageData=this->getImage(source.asInt(),name,&dataLength);
    if(imageData==nullptr){
        return nullptr;
    }


    if (dataLength>=4&&imageData[0]==0x89 && imageData[1]==0x50&&imageData[2]==0x4E&&imageData[3]==0x47){
        return this->getPngRGBA(imageData,dataLength,w,h);
    }
    if (dataLength>=3&&imageData[0]==0xFF && imageData[1]==0xD8&&imageData[2]==0xFF){
        return this->getJpegRGBA(imageData,dataLength,w,h);
    }
    return nullptr;

}

const byte *GltfFile::getBufferView(unsigned int index, unsigned int* length) {
    if (json->empty()) {
        return nullptr;
    }
    auto bufferViews=json->operator[]("bufferViews");
    if(bufferViews.empty()){
        return nullptr;
    }
    auto bufferView=bufferViews[index];
    if(bufferView.empty()){
        return nullptr;
    }
    auto buffer=bufferView["buffer"];
    auto byteLength=bufferView["byteLength"];
    auto byteOffset=bufferView["byteOffset"];

    auto buffers=json->operator[]("buffers");
    if(buffers.empty()){
        return nullptr;
    }
    auto buff=buffers[buffer.asInt()];
    if(buff.empty()){
        return nullptr;
    }
    auto uri=buff["uri"];
    if(uri.empty()){
        if(this->bin==nullptr){
            return nullptr;
        }
        if((byteOffset.asInt()+byteLength.asInt())>this->binLength){
            return nullptr;
        }
        *length=byteLength.asInt();
        return (byte*)&(this->bin[byteOffset.asInt()]);
    }else{
        unsigned int len=0;
        auto d=this->getUriData(uri.asString(),&len);
        if(d==nullptr){
            return nullptr;
        }
        if((byteOffset.asInt()+byteLength.asInt())>len){
            return nullptr;
        }
        *length=byteLength.asInt();
        return (byte*)&(d[byteOffset.asInt()]);
    }
}

const byte *GltfFile::getPngRGBA(const byte* data,unsigned int length,unsigned int *w, unsigned int *h) {
    auto d=render::ImageTool::PngToRGBA(data,length,w,h);
    if(d){
        this->tempData.push_back(d);
        return (const byte*)d;
    }
    return nullptr;


}

const byte *GltfFile::getJpegRGBA(const byte* data,unsigned int length,unsigned int *w,unsigned int *h) {
    auto d=render::ImageTool::JpegToRGBA(data,length,w,h);
    if(d){
        this->tempData.push_back(d);
        return (const byte*)d;
    }
    return nullptr;
}

const byte* GltfFile::getNormal(int index, unsigned int *dataLength) {
    if (json->empty()) {
        return nullptr;
    }
    auto accessors=json->operator[]("accessors");
    if(accessors.empty()){
        return nullptr;
    }
    auto position=accessors[index];
    if(position.empty()){
        return nullptr;
    }
    auto bufferView=position["bufferView"];
    auto componentType=position["componentType"];
    auto count=position["count"];
    auto type=position["type"];
    auto byteOffset=position["byteOffset"].asInt();
    //FLOAT
    if(componentType.asInt()!=5126){
        return nullptr;
    }
    if(type.asString()!="VEC3"){
        return nullptr;
    }
    unsigned int length=0;
    auto d=this->getBufferView(bufferView.asInt(),&length);
    //检测长度 数量*向量长度3*FLOAT长度4
    if((length-byteOffset)<(count.asInt()*3*4)){
        return nullptr;
    }
    *dataLength=(count.asInt()*3*4);
    return &(d[byteOffset]);
}

unsigned int GltfFile::addMesh(render::Scene *sceneResource, int index) {
    unsigned int meshId;
    Json::Value materials=json->operator[]("materials");
    Json::Value meshes=json->operator[]("meshes");
    Json::Value mesh=meshes[index];
    Json::Value material;
    //mesh
    if(!mesh.empty())
    {
        if(!mesh["name"].empty()) {
            meshId=sceneResource->createMesh(mesh["name"].asString());
        }
        auto primitives=mesh["primitives"];
        if(primitives.empty()){
            std::cout<<"can not find mesh.primitives int gltf"<<std::endl;
            return meshId;
        }
        primitives=primitives[0];
        if(primitives.empty()){
            return meshId;
        }
        auto attributes=primitives["attributes"];
        auto indices=primitives["indices"];
        auto materialIndex=primitives["material"];
        if(attributes.empty()){
            return meshId;
        }
        auto position=attributes["POSITION"];
        auto tex_coord=attributes["TEXCOORD_0"];
        auto normal=attributes["NORMAL"];
        //顶点索引
        if(!indices.empty()){
            unsigned int dataLength=0;
            auto data= this->getIndices(indices.asInt(),&dataLength);
            if(data!= nullptr){
                auto i=sceneResource->addBuffer(data,dataLength);
                sceneResource->bindIndices(meshId,i);
            }
        }
        //顶点坐标
        if(!position.empty()){
            unsigned int dataLength=0;
            auto data= this->getPosition(position.asInt(),&dataLength);
            if(data!= nullptr){
                auto i=sceneResource->addVertexBuffer(data,dataLength);
                sceneResource->bindVertex(meshId, i);
            }
        }

        //顶点法线
        if(!normal.empty()){
            unsigned int dataLength=0;
            auto data= this->getNormal(normal.asInt(),&dataLength);
            if(data!= nullptr){
                auto i=sceneResource->addBuffer(data,dataLength);
                sceneResource->bindNormal(meshId, i);
            }
        }


        //顶点uv坐标
        if(!tex_coord.empty()){
            unsigned int dataLength=0;
            auto data= this->getTexCoord(tex_coord.asInt(), &dataLength);
            if(data!= nullptr){
                auto i=sceneResource->addBuffer(data,dataLength);
                sceneResource->bindUvTexture(meshId,i);
            }
        }
        if(!materialIndex.empty()){
            material=materials[materialIndex.asInt()];
        }
    }

    //材质
    unsigned int materialId;

    Json::Value baseColorTexture;
    Json::Value normalTexture;
    Json::Value metallicRoughnessTexture;
    if(!material.empty()){
        if(!material["name"].empty()){
            materialId=sceneResource->createMaterial(material["name"].asString());
        }else{
            materialId=sceneResource->createMaterial("material");
        }
        sceneResource->bindMaterial(meshId,materialId);
        if(!material["normalTexture"].empty()){
            normalTexture=material["normalTexture"]["index"];
        }
        auto pbrMetallicRoughness=material["pbrMetallicRoughness"];
        if(!pbrMetallicRoughness["baseColorTexture"].empty()){
            baseColorTexture=pbrMetallicRoughness["baseColorTexture"]["index"];
        }
        if(!pbrMetallicRoughness["metallicRoughnessTexture"].empty()){
            metallicRoughnessTexture=pbrMetallicRoughness["metallicRoughnessTexture"]["index"];
        }
    }
    //法线贴图
    if(!normalTexture.empty()){
        int i=addTexture(sceneResource,normalTexture.asInt());
        sceneResource->bindNormalTexture(materialId,i);
    }

    //baseColorTexture
    if(!baseColorTexture.empty()){
        int i=addTexture(sceneResource,baseColorTexture.asInt());
        sceneResource->bindBaseColorTexture(materialId,i);
    }

    //metallicRoughnessTexture
    if(!metallicRoughnessTexture.empty()){
        int i=addTexture(sceneResource,metallicRoughnessTexture.asInt());
        sceneResource->bindMetallicRoughnessTexture(materialId,i);
    }

    return meshId;
}

int GltfFile::addTexture(render::Scene *scene, int index) {
    if(textureMap.find(index)!=textureMap.end()){
        return textureMap[index];
    }
    std::string name;
    unsigned int w=0,h=0;
    auto data=getTextureRGBA(index,name,&w,&h);
    if(w>0&&h>0){
        auto texId=scene->addTextureRGBA(name,data,w,h);
        textureMap[index]=(int)texId;
        return (int)texId;
    }
    return 0;

}


