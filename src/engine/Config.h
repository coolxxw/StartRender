//
// Created by xxw on 2023/2/11.
//

#ifndef STARTRENDER_CONFIG_H
#define STARTRENDER_CONFIG_H

#include "../type/type.h"
#include "../math/Vector.h"

//光照参数
struct LightParam{
    Vector3f directionalLight;//平行光方向
    RGBA ambient;//环境光
    RGBA diffuse;//漫反射
    RGBA specular;//镜面光
    int specularExponent;//镜面光指数

    LightParam(){
        directionalLight=Vector3f(-1,0,0);
        ambient=RGBA(255,255,255,0.5);
        diffuse=RGBA(255,255,255,0.5);
        specular=RGBA(255,255,255,0.3);
        specularExponent=64;
    }
};



#endif //STARTRENDER_CONFIG_H
