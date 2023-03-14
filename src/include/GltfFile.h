//
// Created by xxw on 2023/2/16.
//

#ifndef STARTRENDER_GLTFFILE_H
#define STARTRENDER_GLTFFILE_H

#include <unordered_map>
#include "Scene.h"

namespace Json{class Value;}


class GltfFile {
private:
    //gltf引用的外部资源
    struct UriData{
        std::string name;
        unsigned char* data;
        unsigned long long length;
    };

    std::string filePath;
    bool active;
    Json::Value* json;
    Json::Value* jsonBuffers;
    std::unordered_map<std::string,UriData> uriData;
    std::vector<void*> tempData;
    byte *bin;//glb文件内部BIN二进制数据
    int binLength;

private:
    GltfFile(const GltfFile&):GltfFile(){}
    GltfFile &operator=(const GltfFile& g){
        if(&g!=this){
            return *this;
        }
    }

    bool loadGlb(std::ifstream &file);
    bool loadGltf(std::ifstream &file);
    bool loadUriResource(const std::string &uri);
    const byte* getBuffer(unsigned int index,unsigned int *length);
    const byte* getUriData(std::string uri_file, unsigned int *dataLength);

    const byte* getBufferView(unsigned int index,unsigned int *length);
    //accessors
    //获取三角形索引数据 unsigned int表示
    const byte* getIndices(unsigned int index,unsigned int *dataLength);
    //获取顶点数据 float表示
    const byte* getPosition(unsigned int index,unsigned int* dataLength);
    //uv贴图 float表示
    const byte* getTexcoord(unsigned int index,unsigned int* dataLength);
    const byte* getTextureRGBA(unsigned int index,std::string& name,unsigned int *w, unsigned int *h);
    const byte* getJpegRGBA(const byte*data,unsigned int lenght,unsigned int *w, unsigned int *h);
    const byte* getPngRGBA(const byte*index,unsigned int lenght,unsigned int *w, unsigned int *h);
    const byte *getImage(unsigned int index, std::string &name, unsigned int *dataLength);
    const byte *getNormal(int i, unsigned int *dataLenght);


public:
    GltfFile();
    explicit GltfFile(const std::string& filename);
    explicit GltfFile(std::ifstream file);
    ~GltfFile(){close();}

    //从文件打开gltf
    bool load(const std::string& filename);

    bool isValid() const{
        return active;
    }

    //生成场景
    bool fillScene(render::Scene* scene);


    void close();


};


#endif //STARTRENDER_GLTFFILE_H
