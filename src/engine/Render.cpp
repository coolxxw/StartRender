//
// Created by xxw on 2023/2/10.
//

#include "../include/Render.h"
#include "RenderEngine.h"
#include "../engine/platform/SIMDUtil.h"
#include "../include/ContextCache.h"

using  render::StartRender;
using namespace render_core;
using render::Camera;


StartRender::StartRender() {
    engine=new RenderEngine();
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



void StartRender::setSize(int width, int height) {
    engine->contextCacheAlloc->contextCache.width=width;
    engine->contextCacheAlloc->contextCache.height=height;
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

render::CameraController *render::StartRender::getCamera() {
    return (CameraController*)&engine->getContextCache()->camera;
}

render::ContextCache* const render::StartRender::getContext() {
    return engine->getContextCache();
}


