//
// Created by xxw on 2022/11/10.
//

#ifndef STARTRENDER_CONTEXT_H
#define STARTRENDER_CONTEXT_H

#include "data/Camera.h"
#include "Viewing.h"
#include "Config.h"
#include "../include/type.h"
#include "Util.h"
#include "data/Object.h"
#include "data/SceneData.h"

//渲染上下文

namespace render{
    class Accessor;
}
using render::Accessor;

namespace render_core{

    class ContextUpdate{
    public:
        Camera camera;
        int width;
        int height;
        render::LightParam light;
        InOutLock objectLock;
    };


    class Context {
    public:
        Camera camera;
        int width;
        int height;
        render::LightParam light;
        std::vector<Object*> obj;
        ClipSpace clipSpace;
        float * zBuffer;
        RGBA* frameBuffer;
        float *triangularComponentBuffer;//三角形插值分量缓存
        SceneData *scene;
    public:
        Context();
        ~Context();

        void update(ContextUpdate& contextUpdate);

    };

}



#endif //STARTRENDER_CONTEXT_H
