//
// Created by xxw on 2023/3/11.
//

#ifndef STARTRENDER_COLORSHADER_H
#define STARTRENDER_COLORSHADER_H

#include "../data/GBuffer.h"
#include "../data/TextureMap.h"
#include "../data/Environment.h"
#include "../data/CubeMap.h"


namespace render_core {
    class ColorShader {
    public:
        unsigned int width;
        unsigned height;
        Vector3f cameraGear;
        const Environment *lightParam;
        GBufferUnit* gBuffer;
        const CubeMap *skybox;
        void *framebuffer;


    public:
        void shading() const;

        void shadingPbr() const;

        void shadingNormal() const;

        void shadingBaseColor() const;

        void shadingMetal() const;

        void shadingRoughness() const;

        void shadingSkyBox() const;

        static void shadingTexture(unsigned int width, unsigned height, TextureMap texture, void *framebuffer);


    };
} // render_core


#endif //STARTRENDER_COLORSHADER_H
