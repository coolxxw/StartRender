//
// Created by xxw on 2022/11/10.
//

#ifndef STARTRENDER_OBJECT3D_H
#define STARTRENDER_OBJECT3D_H

#include <vector>
#include <string>
#include <memory>

#include "../math/Vector.h"
#include "../object/ObjectFile.h"

//一个三维物体 包括顶点数据 面索引 贴图 uv贴图
class Object3D {
public:

    struct Face{
        int a;
        int b;
        int c;
    };

    //顶点属性
    struct Attributes{
        //uv贴图 uv坐标必须是float 长度等于顶点数量
        Vector2f textureCoord;
    };

    unsigned long long vertexLength;
    unsigned long long faceLength;
    Vector4f *vertex;//原始顶点坐标
    Vector4f* viewVertex;//投影变换后的顶点坐标
    Face *face;//三角形面,索引从0开始
    Vector3f *normalVector;//顶点法线
    Attributes* attributes{};//顶点属性
    Vector3f center{};
    float objectSize{};
    unsigned int diffuseTextureWidth;
    unsigned int diffuseTextureHeight;
    RGBAF *diffuseTexture;

    Object3D();

    explicit Object3D (ObjectFile* obj);


    ~Object3D();

private:

    Object3D(const Object3D&):Object3D(){}//禁止复制
    Object3D& operator=(const Object3D&)=default;//

};


#endif //STARTRENDER_OBJECT3D_H
