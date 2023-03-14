//
// Created by xxw on 2023/2/10.
//

#ifndef STARTRENDER_RENDERFRAME_H
#define STARTRENDER_RENDERFRAME_H

#include "../include/type.h"
#include "Context.h"

namespace render_core{

    class RenderFrame {
    private:
        Context *context;

    public:

        explicit RenderFrame(Context *context);

        void render();

    private:

        void refreshFrameBuffer();
        void refreshZBuffer();
        //对一个三角形面进行光删化
        void viewTransform(Matrix4f matrix);
        void rasterize(int left,int right,int top,int bottom,Vector2f a,Vector2f b,Vector2f c);

    };


}


#endif //STARTRENDER_RENDERFRAME_H
