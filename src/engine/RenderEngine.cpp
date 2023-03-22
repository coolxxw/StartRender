//
// Created by xxw on 2022/11/10.
//
#include <Windows.h>
#include <ctime>

#include <lodepng_util.h>

#include "RenderEngine.h"


using namespace render_core;

bool render_core::RenderEngine::isInit=true;

RenderEngine::RenderEngine() {
    if(!isInit){
        SIMDUtil::getSimdSupportInfo();
    }
    isInit=true;
    frameBufferLock= false;
    maxFPS=60;
    frameCounter=0;
    thread=0;
    threadExitFlag=false;
    paintImpl= nullptr;
    eventQueue=new std::queue<render::Event*>;
    context = new Context();
    contextUpdate= new ContextUpdate;
    contextCacheAlloc= new ContextCacheAlloc();
    createContentCache(context,&contextCacheAlloc->contextCache);
    context->updateSkyboxMap();
    renderFrame = new GraphicsPipeline(context);
    //eventManager=new EventManager(context);
}

RenderEngine::~RenderEngine() {
    while (thread){
        threadExitFlag= true;
    }
    delete renderFrame;
    //delete eventManager;
    delete context;
    delete contextCacheAlloc;
    delete contextUpdate;
    delete eventQueue;
}

void RenderEngine::setMaxFPS(float newFps) {
    this->fps=newFps;
}

static DWORD WINAPI StartRenderThreadProc(
        _In_ LPVOID lpParameter
){
    auto* obj= (RenderEngine*)lpParameter;
    return obj->renderThread();
}

void RenderEngine::startRender() {
    if(thread){
        return;
    }
    threadExitFlag=false;
    thread= ::CreateThread(nullptr, 0, StartRenderThreadProc, this, 0, 0);
}

void RenderEngine::stopRender() {
    threadExitFlag=true;
}



int RenderEngine::renderThread() {
    int frameTimeStart=clock();
    long long frameStart=0;
    while(!threadExitFlag){

        //处理事件
        context->update(contextUpdate,&contextCacheAlloc->contextCache);
        eventHandler();
       // eventManager->disposalAllEvent();

        //限制帧率
        int t=std::clock();
        //while((t-renderTime)<(CLOCKS_PER_SEC/maxFPS )){
        //    ::Sleep(15);
        //    t=std::clock();
       // }

        //帧统计时间
        int fpsInv=1*CLOCKS_PER_SEC;
        if((t-frameTimeStart)>=(fpsInv)){
            fps=(float)(frameCounter-frameStart)/(float)(t-frameTimeStart)*CLOCKS_PER_SEC;
            //std::cout<<fps<<" FPS total:"<<frameCounter<<std::endl;
            frameTimeStart=t;
            frameStart=frameCounter;
        }

        if(context->width*context->height<=0){
            continue;
        }



        //开始渲染
        frameBufferLock= true;
        renderFrame->render();
        frameCounter++;
        if(paintImpl){
            paintImpl->paint(context->frameBuffer,context->width,context->height);
        }
        //结束渲染
        frameBufferLock=false;

    }
    thread= nullptr;
    return 0;
}

float render_core::RenderEngine::getFps() const {
    return fps;
}

long long render_core::RenderEngine::getFrameCounter() const {
    return frameCounter;
}

void render_core::RenderEngine::createContentCache(const Context *context1, render::ContextCache *cache) {
    cache->camera=context1->camera;
    cache->width=context1->width;
    cache->height=context1->height;
    cache->light=context1->light;
}


render::ContextCache *RenderEngine::getContextCache(){
    return &contextCacheAlloc->contextCache;
}

void render_core::RenderEngine::eventHandler() {
    eventQueueLock.lock();
    auto queue=eventQueue;
    eventQueue=new std::queue<render::Event*>;
    eventQueueLock.unlock();
    while(!queue->empty()){
        auto e=queue->front();
        queue->pop();
        switch(e->type){

            case render::Event::UpdateSkybox:
            {
                auto *e2= dynamic_cast<render::UpdateSkyBoxEvent *>(e);
                Texture *texture;
                switch (e2->direction) {
                    case render::UpdateSkyBoxEvent::Left:
                        texture = &context->skyboxTexture.left;
                        break;
                    case render::UpdateSkyBoxEvent::Right:
                        texture = &context->skyboxTexture.right;
                        break;
                    case render::UpdateSkyBoxEvent::Top:
                        texture = &context->skyboxTexture.top;
                        break;
                    case render::UpdateSkyBoxEvent::Bottom:
                        texture = &context->skyboxTexture.bottom;
                        break;
                    case render::UpdateSkyBoxEvent::Front:
                        texture = &context->skyboxTexture.front;
                        break;
                    case render::UpdateSkyBoxEvent::Back:
                        texture = &context->skyboxTexture.back;
                        break;
                }
                delete[] (char*)texture->data;
                texture->data=e2->data;
                texture->width=e2->width;
                texture->height=e2->height;
                context->updateSkyboxMap();
                delete e2;
            }
            break;

        }

        //delete (render::Event*)e;
    }

    delete queue;


}

void render_core::RenderEngine::postEvent(render::Event* e) {
    eventQueueLock.lock();
    eventQueue->push(e);
    eventQueueLock.unlock();
}




