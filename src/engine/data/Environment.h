//
// Created by xxw on 2023/2/21.
//

#ifndef STARTRENDER_ENVIRONMENT_H
#define STARTRENDER_ENVIRONMENT_H


#include "../../math/Vector.h"

namespace render_core {
    //环境参数，包含光照参数
    struct Environment{
        Vector3f directionalLight;//平行光方向
        RGBAF ambient;//环境光
        RGBAF diffuse;//漫反射
        RGBAF specular;//镜面光
        int specularExponent;//镜面光指数

        Environment(){
            directionalLight=Vector3f(0,0,-1);
            ambient=RGBAF(1.0, 1.0, 1.0, 0.5);
            diffuse=RGBAF(1.0, 1.0, 1.0, 0.5);
            specular=RGBAF(1.0, 1.0, 1.0, 0.3);
            specularExponent=64;
        }
    };

} // render

#endif //STARTRENDER_ENVIRONMENT_H
