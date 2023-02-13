//
// Created by xxw on 2022/11/10.
//

#include "Context.h"

Context::Context() {

    ObjectFile file=ObjectFile::LoadFromObjFile("bee.obj");
    Object3D* obj=new Object3D(&file);
    if(obj->faceLength){
        object.push_back(obj);
    }
    width=0;
    height=0;
    zBuffer= nullptr;
    frameBuffer=nullptr;
}

void Context::setSize(int width, int height) {
    if(zBuffer){
        delete zBuffer;
    }
    zBuffer=new float [width*height];

    if(frameBuffer){
       delete frameBuffer;
    }
    frameBuffer=new ARGB[width*height];
    this->width=width;
    this->height=height;


}

Context::~Context() {
    for(Object3D* obj:object){
        delete obj;
    }
}
