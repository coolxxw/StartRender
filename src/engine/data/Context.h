//
// Created by xxw on 2022/11/10.
//

#ifndef STARTRENDER_CONTEXT_H
#define STARTRENDER_CONTEXT_H

#include "Camera.h"
#include "../Viewing.h"
#include "../../include/type.h"
#include "../Util.h"
#include "Object.h"
#include "SceneData.h"
#include "ContextCache.h"
#include "CubeMap.h"

//渲染上下文

namespace render{
    class Accessor;
}
using render::Accessor;

namespace render_core{

    //渲染上下文
    class Context {
    public:
        int width;//帧缓冲宽度
        int height;//帧缓冲高度
        RGBA* frameBuffer;//帧缓冲
        Camera camera;//相机
        render_core::Environment light;
        SceneData *scene;
        std::vector<Object*> obj;
        CubeMap skybox;
        ClipSpace clipSpace;
        float * zBuffer;


    public:
        Context();
        ~Context();

        void update(ContextCache& contextUpdate);

    };

}



#endif //STARTRENDER_CONTEXT_H
