//
// Created by xxw on 2023/3/8.
//

#ifndef STARTRENDER_TEXTURE_H
#define STARTRENDER_TEXTURE_H
#include "../../include/type.h"
namespace render_core{
    struct Texture{
        const RGBA* texture;
        unsigned width;
        unsigned height;

        RGBAF getAttribute(float u,float v){
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

            float h1=u-a;
            float h2=b-u;
            float r1=P1.r*h1+P2.r*h2;
            float g1=P1.g*h1+P2.g*h2;
            float b1=P1.b*h1+P2.b*h2;
            float a1=P1.a*h1+P2.a*h2;

            float r2=P3.r*h1+P4.r*h2;
            float g2=P3.g*h1+P4.g*h2;
            float b2=P3.b*h1+P4.b*h2;
            float a2=P3.a*h1+P4.a*h2;

            h1=v-c;
            h2=d-v;

            r1=r1*h1+r2*h2;
            g1=g1*h1+g2*h2;
            b1=b1*h1+b2*h2;
            a1=a1*h1+a2*h2;

            return RGBAF(r1,g1,b1,a1);
        }
    };




}


#endif //STARTRENDER_TEXTURE_H
