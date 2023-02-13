//
// Created by xxw on 2023/2/10.
//

#ifndef STARTRENDER_RENDERFRAME_H
#define STARTRENDER_RENDERFRAME_H

#include "../type/type.h"
#include "Context.h"

class RenderFrame {
private:
    Context *context;

public:

    RenderFrame(Context *context);

    void render();

private:

    void refreshFrameBuffer();
    void refreshZBuffer();

};


#endif //STARTRENDER_RENDERFRAME_H
