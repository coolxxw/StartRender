//
// Created by xxw on 2023/3/8.
//

#ifndef STARTRENDER_RASTERIZATIONSHADER_H
#define STARTRENDER_RASTERIZATIONSHADER_H

#include "../../math/Vector.h"

namespace render_core {

    class RasterizationShader {
    public:

        unsigned int width;
        unsigned int height;
        float* zBuffer;
        const unsigned int *indices;
        unsigned int indicesCount;
        const Vector4f *vertex;


        void shading();

        void shadingSimpling4();
    };

} // render_core


#endif //STARTRENDER_RASTERIZATIONSHADER_H
