//
// Created by xxw on 2023/3/12.
//

#ifndef STARTRENDER_FRAGMENTSHADER_H
#define STARTRENDER_FRAGMENTSHADER_H


#include "../../math/Vector.h"
#include "../data/GBuffer.h"
#include "../data/Texture.h"
#include "../data/VertexAttribute.h"

namespace render_core {
    class FragmentShader {
    public:

        static void shading(
                unsigned int width,
                unsigned int heigth,
                const float *zbuffer,//输入zbuffer width*height
                const unsigned int *indices,
                unsigned int indicesCount,
                const Vector4f *vertex,
                const Vector4f* preVertex,
                const VertexAttribute *attribute,//2*width*height
                Texture normalTexture,
                Texture baseColorTexture,
                GBufferUnit *gBuffer//输出gbuffer width*height
        );

    };
}



#endif //STARTRENDER_FRAGMENTSHADER_H
