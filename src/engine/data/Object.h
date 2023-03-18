//
// Created by xxw on 2023/3/8.
//

#ifndef STARTRENDER_OBJECT_H
#define STARTRENDER_OBJECT_H

#include "../../math/Vector.h"
#include "TextureMap.h"
#include "VertexAttribute.h"

namespace render_core{
    struct Object{

    public:
        bool valid=false;
        Vector3f centor;
        float size=1;
        bool update=false;
        Matrix4f matrix;
        unsigned int indicesCount=0;
        unsigned int verticesCount=0;
        const unsigned int* indices= nullptr;
        const Vector4f *preVertices= nullptr;//原始坐标
        Vector4f *camVertices= nullptr;//相机坐标系
        Vector4f *vertices= nullptr;
        VertexAttribute *attribute=nullptr;
        Vector3f* normal;
        float* uvTexture{};
        TextureMap normalTexture;
        TextureMap baseColorTexture;
        TextureMap metalRoughnessTexture;
        explicit Object(unsigned int vertexNumber){
            verticesCount=vertexNumber;
            vertices=new Vector4f[vertexNumber];
            camVertices=new Vector4f[vertexNumber];
            attribute= new  VertexAttribute[vertexNumber];
        }
        ~Object(){
            delete vertices;
            delete camVertices;
            delete attribute;
        }
    };


}

#endif //STARTRENDER_OBJECT_H
