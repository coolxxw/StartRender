//
// Created by xxw on 2023/3/13.
//

#ifndef STARTRENDER_VERTEXATTRIBUTE_H
#define STARTRENDER_VERTEXATTRIBUTE_H

#include "../../math/Vector.h"

namespace render_core{
    struct VertexAttribute{
        Vector3f normal;
        float u;
        float v;
    };

}


#endif //STARTRENDER_VERTEXATTRIBUTE_H
