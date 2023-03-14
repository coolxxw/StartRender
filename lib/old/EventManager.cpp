//
// Created by xxw on 2022/11/10.
//

#include "EventManager.h"

using namespace render;

void EventManager::disposalAllEvent() {
    while(true){
        while(postLock);
        disposalLock= true;
        if(postLock){
            disposalLock=false;
        }else{
            break;
        }
    }
    auto list=eventList;
    eventList=new std::vector<Event*>;
    disposalLock= false;

    for(auto e:*list){
        e->exec(context);
        delete e;
    }
    delete list;
}

void EventManager::postEvent(Event *e) {
    while(true){
        while(disposalLock);
        postLock= true;
        if(disposalLock){
            postLock=false;
        }else{
            break;
        }
    }

    eventList->push_back(e);

    postLock= false;
}

EventManager::EventManager(Context *context):context(context) {
    eventList=new std::vector<Event*>;
    postLock= false;
    disposalLock= false;
}

EventManager::~EventManager() {
    delete eventList;
}

void ResetCameraEvent::exec(Context *context) {
    context->camera=this->camera;
}

void ResizeEvent::exec(Context *context) {
    
}
