//
// Created by xxw on 2023/2/16.
//

#ifndef STARTRENDER_GLTFFILE_H
#define STARTRENDER_GLTFFILE_H

#include <unordered_map>
#include "../engine/Object3D.h"

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

    //生成网格数据,包括附加的贴图
    Object3D* newMesh();

    void close();

};


#endif //STARTRENDER_GLTFFILE_H
