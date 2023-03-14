//
// Created by xxw on 2023/3/11.
//

#include "ColorShader.h"
#include "../../include/type.h"

using namespace render_core;



void ColorShader::shadingTexture(unsigned int width, unsigned int height, Texture texture, void *framebuffer) {
    RGBA* frame=(RGBA*)framebuffer;
    for(int i=0;i<height/2 && i<texture.height;i++){
        for (int j=0;j<width && j<texture.width;j++){
            frame[i*width+j]=texture.texture[i*texture.width+j];
        }
    }
}

void ColorShader::shading(unsigned int width, unsigned int height, render::LightParam &lightParam, GBufferUnit *gbuffer,
                          void *framebuffer) {
    auto l=-lightParam.directionalLight;

    RGBA* frame=(RGBA*)framebuffer;
    for(int i=0;i<height;i++){
        for (int j=0;j<width;j++){

            float f=l*gbuffer[i*width+j].normal;
            f=f<0?0:f;
            f=f*0.7+0.3;
            //f=1;
            RGBAF color=gbuffer[i*width+j].baseColor;
            //color=RGBAF(1,1,1,1);


            color.r=(((float)color.r)*f);
            color.g=(((float)color.g)*f);
            color.b=(((float)color.b)*f);
            frame[i*width+j]=RGBA((byte)(color.r*255.f),(byte)(color.g*255.f),(byte)(color.b*255.f),255);
        }
    }
}
