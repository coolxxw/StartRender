//
// Created by xxw on 2023/2/10.
//

#ifndef STARTRENDER_TYPE_H
#define STARTRENDER_TYPE_H

#include "../platform/def.h"

#ifndef byte
typedef unsigned char byte;
#endif



struct RGBA{
    byte r;
    byte g;
    byte b;
    byte a;

    explicit RGBA(byte r=0, byte g=0, byte b=0, byte a=255): a(a), r(r), g(g), b(b){}

};

struct RGBAF{
    float r;
    float g;
    float b;
    float a;

    explicit RGBAF(float r=1.0, float g=1.0, float b=1.0, float a=1.0): r(r), g(g), b(b), a(a){}
};



#endif //STARTRENDER_TYPE_H
