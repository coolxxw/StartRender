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

        inline RGBAF getAttribute2(float u,float v)const{
            u=u*(float)(width-1)+0.5f;
            v=v*(float)(height-1)+0.5f;

            auto b= floor(u);
            auto a= b-1.0f;
            auto d= floor(v);
            auto c= d-1.0f;

            unsigned int y1=((int)d);
            unsigned int x1=((int)b);
            unsigned int y=(y1-1)%height;
            unsigned int x=(x1-1)%width;

            auto P1=texture[y*width+x];
            auto P2=texture[y*width+x1];
            auto P3=texture[y1*width+x];
            auto P4=texture[y1*width+x1];

            const static unsigned long ONE=0x00100;
            const static auto ONEF=(float)ONE;
            const static unsigned long ONE2=ONE*ONE;
            const static auto ONE2F=(float)(ONE2*256);


            //auto h2=(unsigned long)((u-a)*ONEF);
            float bSubU=b-u+1.0f;
            unsigned int floatInt=*(unsigned int*)&bSubU;
            unsigned int h2,h1;
            h1=(floatInt&(0x007FFFFF))>>15;
            h2=h1^0xFF;

            unsigned long r1=h1*P1.r+h2*P2.r;
            unsigned long g1=h1*P1.g+h2*P2.g;
            unsigned long b1=h1*P1.b+h2*P2.b;
            unsigned long a1=h1*P1.a+h2*P2.a;

            unsigned long r2=h1*P3.r+h2*P4.r;
            unsigned long g2=h1*P3.g+h2*P4.g;
            unsigned long b2=h1*P3.b+h2*P4.b;
            unsigned long a2=h1*P3.a+h2*P4.a;

            //h2=(unsigned long)((v-c)*ONEF);
            float dSubV=d-v+1.0f;
            floatInt=*(unsigned int*)&dSubV;
            h1=(floatInt&(0x007FFFFF))>>15;
            //h1=ONE-h2
            h2=h1^0xFF;

            r1=h1*r1+h2*r2;
            g1=h1*g1+h2*g2;
            b1=h1*b1+h2*b2;
            a1=h1*a1+h2*a2;

            return RGBAF(((float)r1)/ONE2F,((float)g1)/ONE2F,((float)b1)/ONE2F,((float)a1)/ONE2F);
        }


        inline RGBAF getAttribute(float u,float v)const{
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
