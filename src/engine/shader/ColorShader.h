//
// Created by xxw on 2023/3/11.
//

#ifndef STARTRENDER_COLORSHADER_H
#define STARTRENDER_COLORSHADER_H

#include "../data/GBuffer.h"
#include "../data/Texture.h"
#include "../Config.h"

namespace render_core {
    class ColorShader {
    public:
        static void shading(unsigned int width,unsigned height,
                            render::LightParam &lightParam,
                            GBufferUnit* gbuffer,void *framebuffer);

        static void shadingTexture(unsigned int width,unsigned height,Texture texture,void *framebuffer);
    };
} // render_core


#endif //STARTRENDER_COLORSHADER_H
