//
// Created by xxw on 2022/11/10.
//

#include <fstream>
#include <sstream>
#include "Object3D.h"
#include "ObjectOperator.h"
#include "../math/Math.h"


Object3D::Object3D() {

    vertexLength=0;
    faceLength=0;
    face=nullptr;
    vertex= nullptr;
    normalVector= nullptr;
    viewVertex= nullptr;
}

Object3D::~Object3D() {
    delete face;
    delete vertex;
    delete normalVector;
    delete viewVertex;

    vertexLength=0;
    faceLength=0;
    face=nullptr;
    vertex= nullptr;
    normalVector= nullptr;
    viewVertex= nullptr;
};

Object3D::Object3D(ObjectFile *obj) {

    vertexLength=obj->vertex.size();
    faceLength=obj->face.size();

    vertex=new Vector4f[vertexLength];
    viewVertex = new Vector4f[vertexLength];
    normalVector=new Vector3f[vertexLength];
    face=new Face[faceLength];

    decimal minX=Math::decimal_max;
    decimal minY=Math::decimal_max;
    decimal minZ=Math::decimal_max;
    decimal maxX=0;
    decimal maxY=0;
    decimal maxZ=0;
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



