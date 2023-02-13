//
// Created by xxw on 2022/11/10.
//

#include "EventManager.h"

void EventManager::disposalAllEvent() {
    for(auto e:eventList){
        e->exec(context);
        delete e;
    }
    eventList.clear();
}

void EventManager::postEvent(Event *e) {
    eventList.push_back(e);
}

void ResetCameraEvent::exec(Context *context) {
    context->camera=this->camera;
}

void ResizeEvent::exec(Context *context) {
    context->setSize(width,height);
}
