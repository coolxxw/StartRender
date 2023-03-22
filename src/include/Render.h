
#ifndef STARTRENDER_RENDER_H
#define STARTRENDER_RENDER_H


#include "CameraController.h"
#include "Scene.h"
#include "ContextCache.h"

namespace render_core{
    class RenderEngine;
}
using render_core::RenderEngine;



namespace render{

    class RenderPaintInterface{
    public:
        virtual void paint(void* frameBuffer,int width,int height)=0;

    };

    class StartRender {
    public:
        RenderEngine *engine;


    public :
        StartRender();
        ~StartRender();
        //注册渲染输出函数
        void registerPaintImpl(RenderPaintInterface *paintImpl);

        long long getFrameCounter() const;

        float getFps() const;

        CameraController *getCamera() const;


        void setSize(int width,int height);

        void start();

        void stop();

        void setScene(render::Scene& scene);

        void addSkyBoxImage(std::string file,std::string direct);
        void addSkyBoxImage(const void* data,unsigned int dataLenght,std::string direct);

        ContextCache *getContext();

    };
};



#endif //STARTRENDER_RENDER_H
