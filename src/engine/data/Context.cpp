//
// Created by xxw on 2022/11/10.
//

#include "Context.h"
#include "../../include/GltfFile.h"

using namespace render_core;

Context::Context() {

    width=0;
    height=0;
    zBuffer= nullptr;
    frameBuffer=nullptr;
    scene=new SceneData;
}



Context::~Context() {
    for(auto i:obj){
        delete i;
    }

    delete zBuffer;
    delete frameBuffer;
    delete scene;
}

void Context::update(ContextCache &contextUpdate) {
    //更新 分辨率
    if(width!=contextUpdate.width || height!=contextUpdate.height){
        width=contextUpdate.width;
        height=contextUpdate.height;

        delete zBuffer;
        delete frameBuffer;


        zBuffer=new float [width*height];
        frameBuffer=new RGBA[width * height];

    }
    //相机
    camera=contextUpdate.camera;
    //光照
    light=contextUpdate.light;
}

