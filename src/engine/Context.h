//
// Created by xxw on 2022/11/10.
//

#ifndef STARTRENDER_CONTEXT_H
#define STARTRENDER_CONTEXT_H

#include "../object/Object3D.h"
#include "../event/Camera.h"
#include "../viewing/Viewing.h"
#include "Config.h"
#include "../type/type.h"

//渲染上下文

class Context {
public:
    std::vector<Object3D*> object;
    Camera camera;
    Camera tempCamera;
    ClipSpace clipSpace;
    int width;
    int height;
    float* zBuffer;
    ARGB* frameBuffer;
    LightParam light;
public:
    Context();
    ~Context();

    void setSize(int width,int height);

};


#endif //STARTRENDER_CONTEXT_H
