//
// Created by xxw on 2022/11/10.
// 渲染事件

#ifndef STARTRENDER_EVENTMANAGER_H
#define STARTRENDER_EVENTMANAGER_H

#include <vector>
#include "RenderEngine.h"

namespace render{

    class EventManager;
    class Event{
    private:
        friend class EventManager;
        virtual void exec(Context* context){};
    };

//以队列的形式保存渲染引擎需要处理的事件
//渲染引擎在帧空隙处理这些事件
    class EventManager {
    private:
        Context *context;
        std::vector<Event*> *eventList;
        volatile bool postLock;
        volatile bool disposalLock;

    public:
        explicit EventManager(Context* context);
        ~EventManager();
        void postEvent(Event *e);
        void disposalAllEvent();
    };

    class ResizeEvent:public Event{
    public:
        int width;
        int height;

        ResizeEvent(int w,int h):width(w),height(h){}

    private:
        void exec(Context *context) override;
    };

    class ResetCameraEvent: public Event{
    public:
        render_core::Camera camera;

        explicit ResetCameraEvent(render_core::Camera camera): camera(camera){}

    private:
        void exec(Context *context) override;

    };


}


#endif //STARTRENDER_EVENTMANAGER_H
