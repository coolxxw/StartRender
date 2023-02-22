//
// Created by xxw on 2022/11/10.
//

#ifndef STARTRENDER_RENDERTHREAD_H
#define STARTRENDER_RENDERTHREAD_H

#include "Context.h"
#include "Camera.h"
#include "../include/type.h"
#include "RenderFrame.h"
#include "../include/Render.h"

namespace RenderCore{

    class EventManager;
    class EventManager;

    class RenderThread {
    public:

        volatile bool frameBufferLock;
        int maxFPS;
        long long frameCounter;//帧计数器
        float fps;

        Render::RenderPaintInterface *paintImpl;
        void *thread;
        volatile bool threadExitFlag;

        Context *context;
        ContextUpdate *contextUpdate;

        RenderFrame *renderFrame;
        //EventManager *eventManager;

        friend class RenderEvent;


    public:
        RenderThread();
        ~RenderThread();

        //注册渲染输出函数
        void registerPaintImpl(Render::RenderPaintInterface *paintImpl){this->paintImpl=paintImpl;}
        //设置帧率上限
        void setMaxFPS(int fps);

        float getFps();

        long long getFrameCounter();

        //启动渲染线程
        void startRender();
        //停止渲染
        void stopRender();

        int renderThread();

    private:




    };


}


#endif //STARTRENDER_RENDERTHREAD_H
