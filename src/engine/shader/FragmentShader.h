//
// Created by xxw on 2023/3/12.
//

#ifndef STARTRENDER_FRAGMENTSHADER_H
#define STARTRENDER_FRAGMENTSHADER_H


#include "../../math/Vector.h"
#include "../data/GBuffer.h"
#include "../data/TextureMap.h"
#include "../data/VertexAttribute.h"
#include "../data/Object.h"
#include "../../include/Config.h"

namespace render_core {
    class FragmentShader {
    public:
        unsigned int width;
        unsigned int height;
        const float *zBuffer;;//输入zbuffer width*height
        const Object* object;
        GBufferUnit *gBuffer;//输出gbuffer width*height
        GBufferCache* gBufferCache;

        render::AntiAliasing antiAliasing;//需要开始MSAA


        void shading();

    private:
        void shadingNoAnti();
        void shadingMSAAx4();

    };
}



#endif //STARTRENDER_FRAGMENTSHADER_H
