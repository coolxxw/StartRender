//
// Created by xxw on 2023/2/21.
//

#ifndef STARTRENDER_LIGHTPARAM_H
#define STARTRENDER_LIGHTPARAM_H

#include "type.h"
#include "../math/Vector.h"

namespace Render {
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

} // Render

#endif //STARTRENDER_LIGHTPARAM_H
