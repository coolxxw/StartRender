//
// Created by xxw on 2022/11/10.
//

#include <fstream>
#include <sstream>
#include "Object3D.h"
#include "../object/ObjectOperator.h"
#include "../math/Math.h"


Object3D::Object3D() {

    vertexLength=0;
    faceLength=0;
    face=nullptr;
    vertex= nullptr;
    normalVector= nullptr;
    viewVertex= nullptr;
    attributes= nullptr;
    diffuseTexture= nullptr;
    diffuseTextureHeight=0;
    diffuseTextureWidth=0;
}

Object3D::~Object3D() {
    delete[] face;
    delete[] vertex;
    delete[] normalVector;
    delete[] viewVertex;
    delete[] attributes;
    delete[] diffuseTexture;

    vertexLength=0;
    faceLength=0;
    face=nullptr;
    vertex= nullptr;
    normalVector= nullptr;
    viewVertex= nullptr;
};

Object3D::Object3D(ObjectFile *obj) {
    face=nullptr;
    vertex= nullptr;
    normalVector= nullptr;
    viewVertex= nullptr;
    diffuseTexture= nullptr;
    diffuseTextureWidth=0;
    diffuseTextureHeight=0;


    vertexLength=obj->vertex.size();
    faceLength=obj->face.size();

    vertex=new Vector4f[vertexLength];
    viewVertex = new Vector4f[vertexLength];
    normalVector=new Vector3f[vertexLength];
    face=new Face[faceLength];

    float minX=Math::float_max;
    float minY=Math::float_max;
    float minZ=Math::float_max;
    float maxX=0;
    float maxY=0;
    float maxZ=0;
    for(int i=0;i<vertexLength;i++){
        vertex[i].x=obj->vertex[i].x;
        vertex[i].y=obj->vertex[i].y;
        vertex[i].z=obj->vertex[i].z;

        if(vertex[i].x > maxX){
            maxX=vertex[i].x;
        }
        if(vertex[i].x < minX){
            minX=vertex[i].x;
        }
        if(vertex[i].y > maxY){
            maxY=vertex[i].y;
        }
        if(vertex[i].y < minY){
            minY=vertex[i].y;
        }
        if(vertex[i].z > maxZ){
            maxZ=vertex[i].z;
        }
        if(vertex[i].z < minZ){
            minZ=vertex[i].z;
        }
    }

    for(int i=0;i<obj->face.size();i++){
        face[i].a=obj->face[i].a;
        face[i].b=obj->face[i].b;
        face[i].c=obj->face[i].c;
    }

    center.x= (minX + maxX) / 2;
    center.y= (minY + maxY) / 2;
    center.z= (minZ + maxZ) / 2;
    objectSize=fmax(fmax((maxX - minX), (maxY - minY)), (maxZ - minZ));


    ObjectOperator opt(this);
    opt.calculateNormalVector();



}



