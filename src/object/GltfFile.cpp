//
// Created by xxw on 2023/2/16.
//

#include <fstream>
#include <iostream>
#include <json/json.h>
#include "GltfFile.h"
#include "ObjectOperator.h"

GltfFile::GltfFile(const std::string& filename) {
    this->active = false;
    this->binLength = 0;
    this->bin = nullptr;
    this->json = nullptr;

    load(filename);
}


GltfFile::GltfFile(std::ifstream file) {
    this->active = false;
    this->binLength = 0;
    this->bin = nullptr;
    this->json = nullptr;

    load(file);
}

bool GltfFile::load(const std::string& filename) {
    std::ifstream file;
    file.open(filename,std::ios_base::in|std::ios_base::binary);
    return load(file);
}

bool GltfFile::load(std::ifstream &file) {
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
    }
}

Object3D *GltfFile::newMesh() {
    //解析第0个scenes
    auto *root=(Json::Value*)this->json;
    Json::Value scenes;
    Json::Value meshes;
    Json::Value nodes;
    Json::Value bufferViews;
    Json::Value buffers;
    Json::Value node;
    Json::Value mesh;

    auto *object=new Object3D;

    try{
        scenes=root->operator[]("scenes");
        meshes =root->operator[]("meshes");
        nodes=root->operator[]("nodes");
        buffers=root->operator[]("buffers");
        bufferViews=root->operator[]("bufferViews");
        int nodeIndex=scenes[0]["nodes"][0].asInt();
        //获取根节点
        node=nodes[nodeIndex];
    }catch (std::exception &e){
        std::cout<<e.what()<<std::endl;
        delete object;
        return nullptr;
    }

    Json::Value children=node["children"];
    for(const auto& child:nodes){
        if(!child["mesh"].empty()){
            mesh=meshes[child["mesh"].asInt()];
            break;
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

    Json::Value faceBufferView=bufferViews[faceAccessor["bufferView"].asInt()];
    {
        int buffer=faceBufferView["buffer"].asInt();
        int byteOffset=faceBufferView["byteOffset"].asInt();
        int byteLength=faceBufferView["byteLength"].asInt();
        int target=faceBufferView["target"].asInt();
        //target 34963 ELEMENT_ARRAY_BUFFER

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

    Json::Value vertexBufferView=bufferViews[vertexAccessor["bufferView"].asInt()];
    {
        int buffer=vertexBufferView["buffer"].asInt();
        int byteOffset=vertexBufferView["byteOffset"].asInt();
        int byteLength=vertexBufferView["byteLength"].asInt();
        int target=vertexBufferView["target"].asInt();

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
    opt.calculateNormalVector();
    opt.calculateCenterAndSize();




    return object;

}
