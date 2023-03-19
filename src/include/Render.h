//
// Created by xxw on 2023/2/10.
//

#ifndef STARTRENDER_RENDER_H
#define STARTRENDER_RENDER_H


#include "CameraController.h"
#include "Scene.h"
#include "ContextCache.h"

namespace render_core{
    class RenderEngine;
}
using render_core::RenderEngine;



namespace render{

    class RenderPaintInterface{
    public:
        virtual void paint(void* frameBuffer,int width,int height)=0;

    };

    class StartRender {
    public:
        RenderEngine *engine;


    public :
        StartRender();
        ~StartRender();
        //注册渲染输出函数
        void registerPaintImpl(RenderPaintInterface *paintImpl);

        long long getFrameCounter();

        float getFps();

        CameraController *getCamera();


        void setSize(int width,int height);

        void start();

        void stop();

        Scene getScene();

        ContextCache *const getContext();

    };
};



#endif //STARTRENDER_RENDER_H
