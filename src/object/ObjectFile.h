//
// Created by xxw on 2023/2/11.
//

#ifndef STARTRENDER_OBJECTFILE_H
#define STARTRENDER_OBJECTFILE_H

#include <vector>
#include <string>
#include "../math/Vector.h"


class ObjectFile {
    struct Face{
        int a;
        int b;
        int c;
    };
public:
    std::vector<Vector3f> vertex;//顶点
    std::vector<Face> face;//面

    ObjectFile();

    static ObjectFile LoadFromObjFile(std::string filename);

    void destroy();

};


#endif //STARTRENDER_OBJECTFILE_H
