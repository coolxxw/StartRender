//
// Created by xxw on 2023/2/10.
//

#ifndef STARTRENDER_TYPE_H
#define STARTRENDER_TYPE_H

#ifndef byte
typedef unsigned char byte;
#endif


struct ARGB{
    byte b;
    byte g;
    byte r;
    byte a;

    explicit ARGB(byte r=0,byte g=0,byte b=0,byte a=255):a(a),r(r),g(g),b(b){}

};

struct RGBA{
    byte r;
    byte g;
    byte b;
    float a;
    explicit RGBA(byte r=0,byte g=0,byte b=0,float a=1.0):a(a),r(r),g(g),b(b){
        if(a>1.0){
            a=1.0;
        }else if(a<0){
            a=0;
        }
    }
};


#endif //STARTRENDER_TYPE_H
