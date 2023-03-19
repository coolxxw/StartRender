//
// Created by xxw on 2023/3/8.
//

#ifndef STARTRENDER_TEXTUREMAP_H
#define STARTRENDER_TEXTUREMAP_H
#include "../../include/type.h"
#include "SceneData.h"

namespace render_core{
    //双线性插值贴图映射
    struct TextureMap{
        TextureMap(){
            width=1;
            height=1;
            static RGBA rgba[2*2]={RGBA(),RGBA(),RGBA(),RGBA()};
            texture=rgba;
        }
        explicit TextureMap(render_core::Texture *pTexture)
        :width(pTexture->width),height(pTexture->height),texture((RGBA*)pTexture->data){}

        const RGBA* texture;
        unsigned width;
        unsigned height;

        RGBAF getAttribute(float u,float v)const{
            u=u*(float)width-0.5f;
            v=v*(float)height-0.5f;

            auto a= floor(u);
            auto b= a+1;
            auto c= floor(v);
            auto d= c+1;


            auto P1=texture[(((int)c)%height)*width+((int)a)%width].toRGBAF();
            auto P2=texture[(((int)c)%height)*width+((int)b)%width].toRGBAF();
            auto P3=texture[(((int)d)%height)*width+((int)a)%width].toRGBAF();
            auto P4=texture[(((int)d)%height)*width+((int)b)%width].toRGBAF();

            float h2=u-a;
            float h1=b-u;
            float r1=P1.r*h1+P2.r*h2;
            float g1=P1.g*h1+P2.g*h2;
            float b1=P1.b*h1+P2.b*h2;
            float a1=P1.a*h1+P2.a*h2;

            float r2=P3.r*h1+P4.r*h2;
            float g2=P3.g*h1+P4.g*h2;
            float b2=P3.b*h1+P4.b*h2;
            float a2=P3.a*h1+P4.a*h2;

            h2=v-c;
            h1=d-v;

            r1=r1*h1+r2*h2;
            g1=g1*h1+g2*h2;
            b1=b1*h1+b2*h2;
            a1=a1*h1+a2*h2;

            return RGBAF(r1,g1,b1,a1);
        }
    };




}


#endif //STARTRENDER_TEXTUREMAP_H
