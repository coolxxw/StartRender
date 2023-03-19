//
// Created by xxw on 2022/11/10.
//

#ifndef STARTRENDER_RENDERENGINE_H
#define STARTRENDER_RENDERENGINE_H

#include "../include/type.h"
#include "data/Context.h"
#include "../include/Camera.h"
#include "../include/Render.h"
#include "../include/ContextCache.h"
#include "GraphicsPipeline.h"

namespace render_core{

    class ContextCacheAlloc{
        void* _;
    public:
        render::ContextCache contextCache;
    };

    class RenderEngine {
    public:
        static bool isInit;

        volatile bool frameBufferLock;
        int maxFPS=60;
        long long frameCounter;//帧计数器
        float fps=0;

        render::RenderPaintInterface *paintImpl;
        void *thread;
        volatile bool threadExitFlag;

        Context *context;
        ContextCacheAlloc *contextCacheAlloc;

        GraphicsPipeline *renderFrame;
        //EventManager *eventManager;

        friend class RenderEvent;


        render::ContextCache *getContextCache();

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

        static void createContentCache(const Context* context1,render::ContextCache* cache);


    };


}


#endif //STARTRENDER_RENDERENGINE_H
