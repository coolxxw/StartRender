//
// Created by xxw on 2023/3/8.
//

#ifndef STARTRENDER_RASTERIZATIONSHADER_H
#define STARTRENDER_RASTERIZATIONSHADER_H

#include "../../math/Vector.h"

namespace render_core {

    class RasterizationShader {
    public:

        static void shading(
                unsigned int width,
                unsigned int heigth,
                float* zbuffer,
                const unsigned int *indices,
                unsigned int indicesCount,
                const Vector4f *vertex
        );

    };

} // render_core


#endif //STARTRENDER_RASTERIZATIONSHADER_H
