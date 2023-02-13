//
// Created by xxw on 2022/11/10.
//

#include <fstream>
#include <sstream>
#include "Object3D.h"


Object3D::Object3D() {

}

Object3D::~Object3D() {

}

Object3D::Object3D(ObjectFile *obj) {

    vertexLength=obj->vertex.size();
    faceLength=obj->face.size();
    Vector3f *vm=new Vector3f [vertexLength];//法向量分子
    float *vn=new float [vertexLength];//法向量分母

    vertex=new Vertex[vertexLength];
    face=new Face[faceLength];

    float minx=FLT_MAX;
    float miny=FLT_MAX;
    float minz=FLT_MAX;
    float maxx=0;
    float maxy=0;
    float maxz=0;
    for(int i=0;i<vertexLength;i++){
        vertex[i].x=obj->vertex[i].x;
        vertex[i].y=obj->vertex[i].y;
        vertex[i].z=obj->vertex[i].z;
        vm[i]=0;
        vn[i]=0;
        if(vertex[i].x>maxx){
            maxx=vertex[i].x;
        }
        if(vertex[i].x<minx){
            minx=vertex[i].x;
        }
        if(vertex[i].y>maxy){
            maxy=vertex[i].y;
        }
        if(vertex[i].y<miny){
            miny=vertex[i].y;
        }
        if(vertex[i].z>maxz){
            maxz=vertex[i].z;
        }
        if(vertex[i].z<minz){
            minz=vertex[i].z;
        }
    }

    center.x=(minx+maxx)/2;
    center.y=(miny+maxy)/2;
    center.z=(minz+maxz)/2;
    objectSize=fmax(fmax((maxx-minx),(maxy-miny)),(maxz-minz));


    for(int i=0;i<faceLength;i++){
        face[i].a=obj->face[i].a;
        face[i].b=obj->face[i].b;
        face[i].c=obj->face[i].c;

        Vector3f a=vertex[face[i].a].toVector3f();
        Vector3f b=vertex[face[i].b].toVector3f();
        Vector3f c=vertex[face[i].c].toVector3f();
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
            vertex[i].normalVector=0;
        }else{
            vertex[i].normalVector=vm[i]*(1/vn[i]);
        }
        vertex[i].normalVector.normalization();
    }





}

