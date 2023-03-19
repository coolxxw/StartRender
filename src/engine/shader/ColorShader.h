//
// Created by xxw on 2023/3/11.
//

#ifndef STARTRENDER_COLORSHADER_H
#define STARTRENDER_COLORSHADER_H

#include "../data/GBuffer.h"
#include "../data/TextureMap.h"
#include "../../include/Environment.h"
#include "../data/CubeMap.h"


namespace render_core {
    using render::Environment;
    class ColorShader {
    public:
        unsigned int width;
        unsigned height;
        Vector3f cameraGear;
        GBufferUnit* gBuffer;
        const CubeMap *skybox;
        void *framebuffer;
        Environment environment;


    public:
        void shading() const;

        void shadingPbr() const;

        void shadingNormal() const;

        void shadingBaseColor() const;

        void shadingMetal() const;

        void shadingRoughness() const;

        void shadingSkyBox() const;

        static void shadingTexture(unsigned int width, unsigned height, TextureMap texture, void *framebuffer);


        void shadingEmission() const;
    };
} // render_core


#endif //STARTRENDER_COLORSHADER_H
