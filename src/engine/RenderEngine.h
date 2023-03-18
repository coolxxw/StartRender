//
// Created by xxw on 2022/11/10.
//

#ifndef STARTRENDER_RENDERENGINE_H
#define STARTRENDER_RENDERENGINE_H

#include "data/Context.h"
#include "data/Camera.h"
#include "../include/type.h"
#include "../../lib/old/RenderFrame.h"
#include "../include/Render.h"
#include "GraphicsPipeline.h"

namespace render_core{

    class EventManager;
    class EventManager;

    class RenderEngine {
    public:
        static bool isInit;

        volatile bool frameBufferLock;
        int maxFPS;
        long long frameCounter;//帧计数器
        float fps{};

        render::RenderPaintInterface *paintImpl;
        void *thread;
        volatile bool threadExitFlag;

        Context *context;
        ContextCache *contextUpdate;

        GraphicsPipeline *renderFrame;
        //EventManager *eventManager;

        friend class RenderEvent;


    public:
        RenderEngine();
        ~RenderEngine();

        //注册渲染输出函数
        void registerPaintImpl(render::RenderPaintInterface *paintImpl){ this->paintImpl=paintImpl;}
        //设置帧率上限
        void setMaxFPS(float newFps);

        float getFps() const;

        long long getFrameCounter() const;

        //启动渲染线程
        void startRender();
        //停止渲染
        void stopRender();

        int renderThread();

    private:




    };


}


#endif //STARTRENDER_RENDERENGINE_H
