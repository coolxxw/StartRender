//
// Created by xxw on 2022/11/10.
//

#include "Context.h"
#include "../object/GltfFile.h"

using namespace RenderCore;

Context::Context() {

    GltfFile file("bee.glb");
    auto *obj=file.newMesh();
    file.close();
    //ObjectFile file=ObjectFile::LoadFromObjFile("jball.ob");
    //auto* obj=new Object3D(&file);
    if(obj&&obj->faceLength){
        object.push_back(obj);
    }
    width=0;
    height=0;
    zBuffer= nullptr;
    frameBuffer=nullptr;
    triangularComponentBuffer= nullptr;
}



Context::~Context() {
    for(Object3D* obj:object){
        delete obj;
    }

    delete zBuffer;
    delete frameBuffer;
    delete triangularComponentBuffer;
}

void Context::update(ContextUpdate &contextUpdate) {
    //更新 分辨率
    if(width!=contextUpdate.width || height!=contextUpdate.height){
        width=contextUpdate.width;
        height=contextUpdate.height;

        delete zBuffer;
        delete frameBuffer;
        delete triangularComponentBuffer;

        zBuffer=new float [width*height];
        frameBuffer=new RGBA[width * height];
        triangularComponentBuffer=new float[width*height*4];
    }
    //相机
    camera=contextUpdate.camera;
    //光照
    light=contextUpdate.light;
    //物体
    contextUpdate.objectLock.inUnlock();

    for(auto o:contextUpdate.object){
        object.push_back(o);
    }
    contextUpdate.object.clear();

    contextUpdate.objectLock.outUnlock();

}

