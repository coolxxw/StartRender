//
// Created by xxw on 2023/2/10.
//

#include "Render.h"

Render::Render(int width, int height) {
    engine=new RenderThread();
    engine->setScreenSize(width,height);
}

void Render::start() {
    engine->startRender();
}

void Render::stop() {
    engine->stopRender();
}

void Render::registerPaintImpl(RenderPaintInterface *paintImpl) {
    {engine->paintImpl=paintImpl;}
}

Camera Render::getCamera() {
    return engine->getCamera();
}

void Render::setCamera(Camera camera) {
    engine->setCamera(camera);
}

