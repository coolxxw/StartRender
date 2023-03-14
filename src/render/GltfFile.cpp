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
    Json::Value node;
    Json::Value mesh;
    Json::Value material;

    try{
        images=root->operator[](("images"));
        materials=root->operator[](("materials"));
        meshes =root->operator[]("meshes");
        nodes=root->operator[]("nodes");
        scenes=root->operator[]("scenes");
        textures=root->operator[]("textures");
        //默认解析第0个scenes
        int nodeIndex=scenes[0]["nodes"][0].asInt();
        //获取根节点
        node=nodes[nodeIndex];
    }catch (std::exception &e){
        std::cout<<e.what()<<std::endl;
        return false;
    }

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
        return false;
    }

    auto meshId=0;
    //mesh
    {
        if(!mesh["name"].empty()) {
            meshId=sceneResource->createMesh(mesh["name"].asString());
        }
        auto primitives=mesh["primitives"];
        if(primitives.empty()){
            std::cout<<"can not find mesh.primitives int gltf"<<std::endl;
            return false;
        }
        primitives=primitives[0];
        if(primitives.empty()){
            return false;
        }
        auto attributes=primitives["attributes"];
        auto indices=primitives["indices"];
        auto materialIndex=primitives["material"];
        if(attributes.empty()){
            return false;
        }
        auto position=attributes["POSITION"];
        auto tex_coord=attributes["TEXCOORD_0"];
        auto normal=attributes["NORMAL"];
        //顶点索引
        if(!indices.empty()){
            unsigned int dataLength=0;
            auto data= this->getIndices(indices.asInt(),&dataLength);
            if(data!= nullptr){
                auto index=sceneResource->addBuffer(data,dataLength);
                sceneResource->bindIndices(meshId,index);
            }
        }
        //顶点坐标
        if(!position.empty()){
            unsigned int dataLength=0;
            auto data= this->getPosition(position.asInt(),&dataLength);
            if(data!= nullptr){
                auto index=sceneResource->addVertexBuffer(data,dataLength);
                sceneResource->bindVertex(meshId, index);
            }
        }

        //顶点法线
        if(!normal.empty()){
            unsigned int dataLength=0;
            auto data= this->getNormal(position.asInt(),&dataLength);
            if(data!= nullptr){
                auto index=sceneResource->addBuffer(data,dataLength);
                sceneResource->bindNormal(meshId, index);
            }
        }


        //顶点uv坐标
        if(!tex_coord.empty()){
            unsigned int dataLength=0;
            auto data= this->getTexCoord(tex_coord.asInt(), &dataLength);
            if(data!= nullptr){
                auto index=sceneResource->addBuffer(data,dataLength);
                sceneResource->bindUvTexture(meshId,index);
            }
        }
        if(!materialIndex.empty()){
            material=materials[materialIndex.asInt()];
        }
    }

    //材质
    auto materialId=0;

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
        unsigned int w=0,h=0;
        std::string name;
        auto data= getTextureRGBA(normalTexture.asInt(),name,&w,&h);
        if(data!=nullptr){
            auto index=sceneResource->addTextureRGBA(name,data,w,h);
            sceneResource->bindNormalTexture(materialId,index);
        }

    }

    //baseColorTexture
    if(!baseColorTexture.empty()){
        unsigned int w=0,h=0;
        std::string name;
        auto data= getTextureRGBA(baseColorTexture.asInt(),name,&w,&h);
        if(data!=nullptr){

        }
        auto index=sceneResource->addTextureRGBA(name,data,w,h);
        sceneResource->bindBaseColorTexture(materialId,index);
    }
    //metallicRoughnessTexture
    if(!metallicRoughnessTexture.empty()){
        unsigned int w=0,h=0;
        std::string name;
        auto data= getTextureRGBA(metallicRoughnessTexture.asInt(),name,&w,&h);
        if(data!=nullptr){
            auto index=sceneResource->addTextureRGBA(name,data,w,h);
            sceneResource->bindMetallicRoughnessTexture(materialId,index);
        }

    }
    return true;
}

const byte *GltfFile::getUriData(std::string uri_file, unsigned int *dataLength) {
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
        if(length<(count.asInt()*2)){
            return nullptr;
        }
        int count_int=count.asInt();
        auto bin=new unsigned int[count_int];
        for(int i=0;i<count_int;i++){
            bin[i]=((unsigned short *)d)[i];
        }
        this->tempData.push_back((void*)bin);
        *dataLength=count_int*4;
        return (byte*)bin;
    }else if(componentType.asInt()==5125){//unsigned int
        //检测长度 数量*标量1*UNSIGNED INT长度4
        if(length<(count.asInt()*4)){
            return nullptr;
        }
        *dataLength=length;
        return d;
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
    std::vector<unsigned char > png ;
    lodepng::decode(png,*w,*h,data,length);
    if(*w>0 && *h>0){
        auto* buffer=new unsigned char [(*w)*(*h)*4];
        this->tempData.push_back(buffer);


        return buffer;
    }

    return nullptr;


}

const byte *GltfFile::getJpegRGBA(const byte* data,unsigned int length,unsigned int *w,unsigned int *h) {

    //调用libjpeg turbo
    auto tjHandle=tjInitDecompress();
    if(tjHandle==0){
        return nullptr;
    }
    long unsigned int jpegSize=length; //!< _jpegSize from above
    auto* compressedImage=new unsigned char [length]; //!< _compressedImage from above
    memmove(compressedImage,data,length);

    int jpegSubsamp, width, height;
    tjDecompressHeader(tjHandle,compressedImage,jpegSize,&width,&height);
    int bufferSize=width*height*4;
    auto* buffer=new unsigned char [width*height*4];
    tjDecompress(tjHandle,compressedImage,jpegSize,buffer,width,0,height,4,TJFLAG_FASTDCT);
    delete[] compressedImage;
    tjDestroy(tjHandle);

    *w=width;
    *h=height;
    this->tempData.push_back(buffer);
    return buffer;

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
    return d+byteOffset;
}

