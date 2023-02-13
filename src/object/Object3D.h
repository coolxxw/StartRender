//
// Created by xxw on 2022/11/10.
//

#ifndef STARTRENDER_OBJECT3D_H
#define STARTRENDER_OBJECT3D_H

#include <vector>
#include <string>
#include "../math/Vector.h"
#include "ObjectFile.h"

class Object3D {
public:

    struct Face{
        int a;
        int b;
        int c;
    };
    struct Vertex{
        float x;
        float y;
        float z;
        Vector3f normalVector;//法向量

        Vector3f toVector3f() const{
            return ::Vector3f(x,y,z);
        }
    };


    int vertexLength;
    int faceLength;
    Vertex *vertex;//顶点
    Face *face;//面
    Vector3f center;
    float objectSize;

    Object3D();

    Object3D (ObjectFile* obj);

    ~Object3D();

};


#endif //STARTRENDER_OBJECT3D_H
