//
// Created by xxw on 2022/11/10.
//

#ifndef STARTRENDER_RENDERTHREAD_H
#define STARTRENDER_RENDERTHREAD_H

#include "Context.h"
#include "../event/Camera.h"
#include "../type/type.h"
#include "RenderFrame.h"



class RenderPaintInterface{
public:
    virtual void paint(void* frameBuffer)=0;
};

class EventManager;
class EventManager;

class RenderThread {
public:

    volatile bool frameBufferLock;
    int maxFPS;
    long long frameCounter;//帧计数器

    RenderPaintInterface *paintImpl;
    void *thread;
    volatile bool threadExitFlag;

    Context *context;
    RenderFrame *renderFrame;

    EventManager *eventManager;

    friend class RenderEvent;

public:
    RenderThread();
    ~RenderThread();

    void setScreenSize(int width,int height);
    Camera getCamera();
    void setCamera(Camera camera);


    //注册渲染输出函数
    void registerPaintImpl(RenderPaintInterface *paintImpl){this->paintImpl=paintImpl;}
    //设置帧率上限
    void setMaxFPS(int fps);
    //启动渲染线程
    void startRender();
    //停止渲染
    void stopRender();

    int renderThread();

private:




};


#endif //STARTRENDER_RENDERTHREAD_H
