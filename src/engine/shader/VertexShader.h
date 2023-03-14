//
// Created by xxw on 2023/3/8.
//

#ifndef STARTRENDER_VERTEXSHADER_H
#define STARTRENDER_VERTEXSHADER_H
#include "../../math/Matrix.h"
#include "../data/VertexAttribute.h"

namespace render_core{
    class VertexShader{
    public:

        //顶点着色器 转换顶点坐标
        static void shading(
                const Vector4f* in,
                Vector4f* out,
                unsigned int count,
                const Matrix4f* matrix
                );

        static void vertexAttributeShading(
                    unsigned int indicesCount,
                    const unsigned int* indices,
                    unsigned int vertexCount,
                    const Vector4f* vertices,
                    const Vector3f* normal,
                    const float* uv,
                    VertexAttribute* attribute
                );
    };
}


#endif //STARTRENDER_VERTEXSHADER_H
