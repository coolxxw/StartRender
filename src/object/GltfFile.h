//
// Created by xxw on 2023/2/16.
//

#ifndef STARTRENDER_GLTFFILE_H
#define STARTRENDER_GLTFFILE_H

#include "Object3D.h"

class GltfFile {
private:
    bool active;
    void* json;
    void* bin;
    int binLength;


public:
    explicit GltfFile(const std::string& filename);
    explicit GltfFile(std::ifstream file);
    ~GltfFile(){close();}

    bool load(const std::string& filename);
    bool load(std::ifstream &file);
    bool isValid() const{
        return active;
    }

    //导出顶点数据
    Object3D* newMesh();

    void close();



};


#endif //STARTRENDER_GLTFFILE_H
