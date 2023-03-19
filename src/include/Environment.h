//
// Created by xxw on 2023/2/21.
//

#ifndef STARTRENDER_ENVIRONMENT_H
#define STARTRENDER_ENVIRONMENT_H


#include "../math/Vector.h"

namespace render {
    //环境参数，包含光照参数
    struct Environment{

        //以下参数允许在渲染时实时调整
        //RGBAF总的a允许>1 rgb通道应在0~1之间

        //直射光方向
        Vector3f directLightVector;

        //直射光颜色
        RGBAF directLightColor;

        //直射光漫反射系数
        RGBAF directLightDiffuseFact;

        //直射光镜面光系数
        RGBAF directLightSpecularFact;

        //非直射光系数，计算最终颜色时先用a通道乘rgb再乘最终颜色
        RGBAF indirectLightDiffuseFact;

        RGBAF indirectLightSpecularFact;

        //自发光系数
        RGBAF emissionLightFact;

        Environment(){
            //默认定义

            //默认-z轴
            directLightVector=Vector3f(0,0,-1);
            //直射光 白色
            directLightColor=RGBAF(1.0f,1.0f,1.0f,1.0f);
            directLightDiffuseFact=RGBAF(1.0f,1.0f,1.0f,0.8f);
            directLightSpecularFact=RGBAF(1.0f,1.0f,1.0f,0.8f);
            indirectLightDiffuseFact=RGBAF(1.0,1.0,1.0,0.8);
            indirectLightSpecularFact=RGBAF(1.0,1.0,1.0,0.8);
            emissionLightFact=RGBAF(1.0f,1.0f,1.0f,0.8f);
        }

    };

} // render

#endif //STARTRENDER_ENVIRONMENT_H
