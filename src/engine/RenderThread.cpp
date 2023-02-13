//
// Created by xxw on 2022/11/10.
//
#include <Windows.h>
#include <ctime>
#include <iostream>

#include "RenderThread.h"
#include "../event/EventManager.h"


RenderThread::RenderThread() {
    frameBufferLock= false;
    maxFPS=60;
    frameCounter=0;
    thread=0;
    threadExitFlag=false;
    paintImpl= nullptr;
    context = new Context();
    renderFrame = new RenderFrame(context);
    eventManager=new EventManager(context);
}

RenderThread::~RenderThread() {
    while (thread){
        threadExitFlag= true;
    }
    delete eventManager;
    delete context;
}

void RenderThread::setMaxFPS(int fps) {

}

static DWORD WINAPI ThreadProc(
        _In_ LPVOID lpParameter
){
    RenderThread* obj= (RenderThread*)lpParameter;
    return obj->renderThread();
}

void RenderThread::startRender() {
    if(thread){
        return;
    }
    threadExitFlag=false;
    thread=::CreateThread(0,0,ThreadProc,this,0,0);
}

void RenderThread::stopRender() {
    threadExitFlag=true;
}

void RenderThread::setScreenSize(int width, int height) {
    if(frameBufferLock){
        eventManager->postEvent(new ResizeEvent(width,height));
        return;
    }else{
        context->setSize(width, height);
    }
}

int RenderThread::renderThread() {
    int renderTime=0;
    int frameTimeStart=clock();
    long long frameStart=0;
    while(!threadExitFlag){

        //处理事件
        context->camera=context->tempCamera;
        eventManager->disposalAllEvent();

        //限制帧率
        int t=std::clock();
        //while((t-renderTime)<(CLOCKS_PER_SEC/maxFPS )){
        //    ::Sleep(15);
        //    t=std::clock();
       // }
        renderTime=t;

        //帧统计时间
        int fpsInv=2*CLOCKS_PER_SEC;
        if((t-frameTimeStart)>=(fpsInv)){
            float fps=(float)(frameCounter-frameStart)/(t-frameTimeStart)*CLOCKS_PER_SEC;
            std::cout<<fps<<" FPS total:"<<frameCounter<<std::endl;
            frameTimeStart=t;
            frameStart=frameCounter;
        }


        //开始渲染
        frameBufferLock= true;
        renderFrame->render();
        frameCounter++;
        if(paintImpl){
            paintImpl->paint(context->frameBuffer);
        }
        //结束渲染
        frameBufferLock=false;

    }
    thread= nullptr;
    return 0;
}


void RenderThread::setCamera(Camera camera) {
    context->tempCamera=camera;
}

Camera RenderThread::getCamera() {
    return context->tempCamera;
}




