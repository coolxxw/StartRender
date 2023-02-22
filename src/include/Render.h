//
// Created by xxw on 2023/2/10.
//

#ifndef STARTRENDER_RENDER_H
#define STARTRENDER_RENDER_H


#include "Camera.h"
#include "LightParam.h"

namespace RenderCore{
    class RenderThread;
    class Context;
    class ContextUpdate;
}
using RenderCore::RenderThread;
using RenderCore::Context;
using RenderCore::ContextUpdate;


namespace Render{

    class RenderPaintInterface{
    public:
        virtual void paint(void* frameBuffer,int width,int height)=0;

    };

    class StartRender {
    private:
        RenderThread *engine;


    public :
        StartRender(int width,int height);
        ~StartRender();
        //注册渲染输出函数
        void registerPaintImpl(RenderPaintInterface *paintImpl);

        long long getFrameCounter();

        float getFps();

        Camera getCamera();

        LightParam& getLightParam();

        void setSize(int width,int height);

        void start();

        void stop();

    };
};



#endif //STARTRENDER_RENDER_H
