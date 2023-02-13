//
// Created by xxw on 2023/2/10.
//

#ifndef STARTRENDER_RENDER_H
#define STARTRENDER_RENDER_H

#include "RenderThread.h"


class Render {
private:
    RenderThread *engine;


public :
    Render(int width,int height);
    //注册渲染输出函数
    void registerPaintImpl(RenderPaintInterface *paintImpl);
    Camera getCamera();
    void setCamera(Camera camera);


    void start();

    void stop();

};


#endif //STARTRENDER_RENDER_H
