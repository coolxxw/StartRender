//
// Created by xxw on 2023/2/10.
//

#ifndef STARTRENDER_RENDER_H
#define STARTRENDER_RENDER_H


#include "Camera.h"
#include "Scene.h"

namespace render_core{
    class RenderEngine;
    class Context;
    class ContextCache;
}
using render_core::RenderEngine;
using render_core::Context;
using render_core::ContextCache;


namespace render{

    class RenderPaintInterface{
    public:
        virtual void paint(void* frameBuffer,int width,int height)=0;

    };

    class StartRender {
    public:
        RenderEngine *engine;


    public :
        StartRender(int width,int height);
        ~StartRender();
        //注册渲染输出函数
        void registerPaintImpl(RenderPaintInterface *paintImpl);

        long long getFrameCounter();

        float getFps();

        Camera getCamera();


        void setSize(int width,int height);

        void start();

        void stop();

        Scene getScene();

    };
};



#endif //STARTRENDER_RENDER_H
