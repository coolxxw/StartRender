//
// Created by xxw on 2023/3/12.
//

#ifndef STARTRENDER_GBUFFER_H
#define STARTRENDER_GBUFFER_H

#include "../../math/Vector.h"

namespace render_core{
    struct GBufferUnit{
        Vector3f normal;
        RGBAF baseColor;
    };


}




#endif //STARTRENDER_GBUFFER_H
