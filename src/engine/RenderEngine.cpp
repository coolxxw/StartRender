//
// Created by xxw on 2022/11/10.
//
#include <Windows.h>
#include <ctime>

#include <lodepng_util.h>

#include "RenderEngine.h"
#include "../../lib/old/RenderFrame.h"

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
    context = new Context();
    contextUpdate=new ContextCache();
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
    delete contextUpdate;
}

void RenderEngine::setMaxFPS(float newFps) {
    this->fps=newFps;
}

static DWORD WINAPI ThreadProc(
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
    thread=::CreateThread(nullptr,0,ThreadProc,this,0,0);
}

void RenderEngine::stopRender() {
    threadExitFlag=true;
}



int RenderEngine::renderThread() {
    int frameTimeStart=clock();
    long long frameStart=0;
    while(!threadExitFlag){

        //处理事件
        context->update(*contextUpdate);
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






