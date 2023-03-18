//
// Created by xxw on 2023/2/10.
//

#include "../include/Render.h"
#include "RenderEngine.h"
#include "../platform/SIMDUtil.h"

using  render::StartRender;
using namespace render_core;


StartRender::StartRender(int width, int height) {
    engine=new RenderEngine();
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

render::Camera StartRender::getCamera() {
    return render::Camera(engine->contextUpdate);
}


void StartRender::setSize(int width, int height) {
    engine->contextUpdate->width=width;
    engine->contextUpdate->height=height;
}

long long render::StartRender::getFrameCounter() {
    return engine->getFrameCounter();
}

float render::StartRender::getFps() {
    return engine->getFps();
}


render::Scene render::StartRender::getScene() {
    return Scene((SceneData*)engine->context->scene);
}


