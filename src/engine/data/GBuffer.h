//
// Created by xxw on 2023/3/12.
//

#ifndef STARTRENDER_GBUFFER_H
#define STARTRENDER_GBUFFER_H

#include "../../math/Vector.h"

namespace render_core{
    struct GBufferUnit{
        bool valid;
        Vector3f normal;
        RGBAF baseColor;//基础颜色或反射率
        float metallic;//金属度
        float roughness;//主观粗糙度
        RGBAF emission;//自发光
    };


}




#endif //STARTRENDER_GBUFFER_H
