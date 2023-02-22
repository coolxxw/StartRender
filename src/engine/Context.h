//
// Created by xxw on 2022/11/10.
//

#ifndef STARTRENDER_CONTEXT_H
#define STARTRENDER_CONTEXT_H

#include "../object/Object3D.h"
#include "Camera.h"
#include "Viewing.h"
#include "Config.h"
#include "../include/type.h"
#include "Util.h"

//渲染上下文

namespace Render{
    class Accessor;
}
using Render::Accessor;

namespace RenderCore{

    class ContextUpdate{
    public:
        Camera camera;
        int width;
        int height;
        Render::LightParam light;
        std::vector<Object3D*> object;
        InOutLock objectLock;

    };


    class Context {
    public:
        Camera camera;
        int width;
        int height;
        Render::LightParam light;
        std::vector<Object3D*> object;
        ClipSpace clipSpace;
        decimal * zBuffer;
        ARGB* frameBuffer;
        decimal *triangularComponentBuffer;//三角形插值分量缓存
    public:
        Context();
        ~Context();

        void update(ContextUpdate& contextUpdate);

    };

}



#endif //STARTRENDER_CONTEXT_H
