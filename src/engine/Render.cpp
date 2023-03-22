//
// Created by xxw on 2023/2/10.
//

#include "../include/Render.h"
#include "RenderEngine.h"
#include "../engine/platform/SIMDUtil.h"
#include "../include/ContextCache.h"
#include "render/ImageTool.h"

using render::StartRender;
using namespace render_core;
using render::Camera;


StartRender::StartRender() {
    engine = new RenderEngine();
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
    { engine->paintImpl = paintImpl; }
}


void StartRender::setSize(int width, int height) {
    engine->contextCacheAlloc->contextCache.width = width;
    engine->contextCacheAlloc->contextCache.height = height;
}

long long render::StartRender::getFrameCounter() const {
    return engine->getFrameCounter();
}

float render::StartRender::getFps() const {
    return engine->getFps();
}


render::CameraController *render::StartRender::getCamera() const {
    return (CameraController *) &engine->getContextCache()->camera;
}

render::ContextCache *render::StartRender::getContext() {
    return engine->getContextCache();
}

void render::StartRender::setScene(render::Scene &scene) {
    engine->contextUpdate->sceneData = scene.moveSceneData();
    engine->contextUpdate->updateScene = true;
}

void render::StartRender::addSkyBoxImage(std::string file, std::string direct) {

    unsigned int width, height;
    auto data = ImageTool::imageToRGBA(file, &width, &height);
    if (data == nullptr) {
        return;
    }

    UpdateSkyBoxEvent* event;
    UpdateSkyBoxEvent::Direction direction;

    if (direct == "left") {
        direction=UpdateSkyBoxEvent::Direction::Left;
    } else if (direct == "right") {
        direction=UpdateSkyBoxEvent::Direction::Right;
    } else if (direct == "top") {
        direction=UpdateSkyBoxEvent::Direction::Top;
    } else if (direct == "bottom") {
        direction=UpdateSkyBoxEvent::Direction::Bottom;
    } else if (direct == "front") {
        direction=UpdateSkyBoxEvent::Direction::Front;
    } else if (direct == "back") {
        direction=UpdateSkyBoxEvent::Direction::Back;
    }
    event=new UpdateSkyBoxEvent(direction,height,width,data);
    engine->postEvent(event);

}

void render::StartRender::addSkyBoxImage(const void *d, unsigned int dataLenght, std::string direct) {
    unsigned int width, height;
    auto data = ImageTool::imageToRGBA(d,dataLenght, &width, &height);
    if (data == nullptr) {
        return;
    }

    UpdateSkyBoxEvent* event;
    UpdateSkyBoxEvent::Direction direction;

    if (direct == "left") {
        direction=UpdateSkyBoxEvent::Direction::Left;
    } else if (direct == "right") {
        direction=UpdateSkyBoxEvent::Direction::Right;
    } else if (direct == "top") {
        direction=UpdateSkyBoxEvent::Direction::Top;
    } else if (direct == "bottom") {
        direction=UpdateSkyBoxEvent::Direction::Bottom;
    } else if (direct == "front") {
        direction=UpdateSkyBoxEvent::Direction::Front;
    } else if (direct == "back") {
        direction=UpdateSkyBoxEvent::Direction::Back;
    }
    event=new UpdateSkyBoxEvent(direction,height,width,data);
    engine->postEvent(event);
}


