//
// Created by xxw on 2023/2/10.
//

#ifndef STARTRENDER_TYPE_H
#define STARTRENDER_TYPE_H

#include "../platform/def.h"

#ifndef byte
typedef unsigned char byte;
#endif

struct RGBAF{
    float r;
    float g;
    float b;
    float a;

    explicit RGBAF(float r=1.0, float g=1.0, float b=1.0, float a=1.0): r(r), g(g), b(b), a(a){}
};


struct RGBA{
    byte r;
    byte g;
    byte b;
    byte a;

    explicit RGBA(byte r=0, byte g=0, byte b=0, byte a=255): a(a), r(r), g(g), b(b){}



    //rgbaf值应介于0.0f~1.0f
    explicit RGBA(const RGBAF rgbaf){
        r=(byte)(rgbaf.r*255.0f);
        g=(byte)(rgbaf.g*255.0f);
        b=(byte)(rgbaf.b*255.0f);
        a=(byte)(rgbaf.a*255.0f);
    };

    RGBAF toRGBAF() const{
        return RGBAF((float)r/255.f,(float)g/255.f,(float)b/255.f,(float)a/255.f);
    }


};




#endif //STARTRENDER_TYPE_H
