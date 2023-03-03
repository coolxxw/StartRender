//
// Created by xxw on 2023/2/19.
//

#include "ObjectOperator.h"
#include "../math/Math.h"

void ObjectOperator::calculateNormalVector() {
    if(object== nullptr ) return;


    auto vertex=object->vertex;
    auto face=object->face;
    auto faceLength=object->faceLength;
    auto vertexLength=object->vertexLength;
    auto normalVector=object->normalVector;

    if(faceLength==0) return;

    auto *vn=new float[vertexLength];
    auto* vm=new Vector3f[vertexLength];

    for(int i=0;i<object->faceLength;i++){

        Vector3f a=vertex[face[i].a].vec3();
        Vector3f b=vertex[face[i].b].vec3();
        Vector3f c=vertex[face[i].c].vec3();
        Vector3f va=c-a;
        Vector3f vb=a-b;
        //计算叉积
        Vector3f x=va.cross(vb);
        //面积等于向量长度一半
        float s=sqrt(x.x*x.x+x.y*x.y+x.z*x.z);
        //面积数作为权重
        x=x;
        vn[face[i].a]+=s;
        vn[face[i].b]+=s;
        vn[face[i].c]+=s;
        vm[face[i].a]=vm[face[i].a]+x;
        vm[face[i].b]=vm[face[i].b]+x;
        vm[face[i].c]=vm[face[i].c]+x;

    }

    for(int i=0;i<vertexLength;i++){
        if(vn[i]==0){
            normalVector[i]=Vector3f{0};;
        }else{
            normalVector[i]=vm[i]*(1/vn[i]);
        }
        normalVector[i].normalization();
    }


    delete[] vn;
    delete[] vm;
}



void ObjectOperator::calculateCenterAndSize() {
    if(object== nullptr ) return;

    auto vertex=object->vertex;
    auto vertexLength=object->vertexLength;


    float minX=Math::float_max;
    float minY=Math::float_max;
    float minZ=Math::float_max;
    float maxX=0;
    float maxY=0;
    float maxZ=0;

    for(int i=0;i<vertexLength;i++){
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

    object->center.x= (minX + maxX) / 2;
    object->center.y= (minY + maxY) / 2;
    object->center.z= (minZ + maxZ) / 2;
    object->objectSize=fmax(fmax((maxX - minX), (maxY - minY)), (maxZ - minZ));


}

