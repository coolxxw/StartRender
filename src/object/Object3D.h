//
// Created by xxw on 2022/11/10.
//

#ifndef STARTRENDER_OBJECT3D_H
#define STARTRENDER_OBJECT3D_H

#include <vector>
#include <string>
#include <memory>

#include "../math/Vector.h"
#include "ObjectFile.h"

class Object3D {
public:

    struct Face{
        int a;
        int b;
        int c;
    };

    int vertexLength;
    int faceLength;
    Vector4f *vertex;//顶点
    Vector4f* viewVertex;
    Face *face;//面
    Vector3f *normalVector;
    Vector3f center{};
    float objectSize{};

    Object3D();

    explicit Object3D (ObjectFile* obj);


    ~Object3D();

private:

    Object3D(const Object3D&):Object3D(){}//禁止复制
    Object3D& operator=(const Object3D&)=default;//

};


#endif //STARTRENDER_OBJECT3D_H
