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
    delete gBuffer;
    delete gBuffer;
    delete scene;
}

void Context::update(const render::ContextCache* contextUpdate) {
    //更新 分辨率
    if(width!=contextUpdate->width || height!=contextUpdate->height){
        width=contextUpdate->width;
        height=contextUpdate->height;

        delete zBuffer;
        delete frameBuffer;
        delete gBuffer;


        zBuffer=new float [width*height];
        gBuffer=new GBufferUnit [width*height];
        frameBuffer=new RGBA[width * height];

    }
    //相机
    camera=contextUpdate->camera;
    //光照
    light=contextUpdate->light;

    if(config.antiAliasing!=contextUpdate->config.antiAliasing){
        delete zBuffer;
        if(contextUpdate->config.antiAliasing==render::AntiAliasing::MSAAx4){
            zBuffer=new float [width*height*4];
        }else{
            zBuffer=new float [width*height];
        }
    }

    config=contextUpdate->config;
}

