//
// Created by xxw on 2023/2/10.
//

#include "../include/Render.h"
#include "RenderThread.h"
#include "../platform/SIMDUtil.h"

using  Render::StartRender;
using namespace RenderCore;

StartRender::StartRender(int width, int height) {
    engine=new RenderThread();
    engine->contextUpdate->width=width;
    engine->contextUpdate->height=height;
}

StartRender::~StartRender() {
    delete engine;
}

void StartRender::start() {
    engine->startRender();
}

void StartRender::stop() {
    engine->stopRender();
}

void StartRender::registerPaintImpl(RenderPaintInterface *paintImpl) {
    {engine->paintImpl=paintImpl;}
}

Render::Camera StartRender::getCamera() {
    return Render::Camera(engine->contextUpdate);
}


void StartRender::setSize(int width, int height) {
    engine->contextUpdate->width=width;
    engine->contextUpdate->height=height;
}

long long Render::StartRender::getFrameCounter() {
    return engine->getFrameCounter();
}

float Render::StartRender::getFps() {
    return engine->getFps();
}

Render::LightParam& Render::StartRender::getLightParam() {
    return engine->contextUpdate->light;
}


