//
// Created by xxw on 2023/3/8.
//

#include "GraphicsPipeline.h"
#include "shader/VertexShader.h"
#include "shader/ColorShader.h"
#include "shader/FragmentShader.h"

void render_core::GraphicsPipeline::render() {
    prepare();

    unsigned int sampling=1;
    if(context->config.antiAliasing==render::AntiAliasing::MSAAx4){
        sampling=4;
    }

    int width=context->width;
    int height=context->height;

    ColorShader colorShader;
    colorShader.width=width;
    colorShader.height=height;
    colorShader.framebuffer=context->frameBuffer;
    colorShader.gBuffer=context->gBuffer;
    colorShader.environment=context->light;
    colorShader.cameraGear=context->camera.g;
    colorShader.skybox=&context->skybox;


    if (0&&this->context->obj.size()>=2){

        ColorShader::shadingTexture(width,height,context->obj[1]->baseColorTexture,context->frameBuffer);
         return;
    }



    auto light=context->light;//光源
    Vector3f eye=-context->camera.g;


    //投影矩阵计算
    {

        Viewing viewing(context->camera,context->clipSpace,width,height);
        Matrix4f matrix;

        if(1){
            auto &sceneMax=context->sceneParam.sceneMax;
            auto &sceneMin=context->sceneParam.sceneMin;
            Vector3f v=sceneMax-sceneMin;
            Vector3f centor=(sceneMax+sceneMin)*0.5;
            float size= sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
            {
                float scale=1.5f/size;
                matrix.move(-centor);
                matrix.scale(Vector3f(scale,scale,scale));
            }
        }

        matrix=viewing.matrix*matrix;
        Viewing camTrans=viewing;
        camTrans.matrix=viewing.cam;

        for(int i=1;i<context->obj.size();i++){
            auto object=context->obj[i];
            //顶点着色器  主要完成仿射变换
            VertexShader::shading(object->preVertices,object->vertices,object->verticesCount,&matrix);
        }
    }

    //光栅化+Z测试
    RasterizationShader rasterizationShader;
    rasterizationShader.width=width;
    rasterizationShader.height=height;
    rasterizationShader.zBuffer=context->zBuffer;
    for(int i=1;i<context->obj.size();i++){
        auto object=context->obj[i];
        rasterizationShader.indicesCount=object->indicesCount;
        rasterizationShader.indices=object->indices;
        rasterizationShader.vertex=object->vertices;

        if(context->config.antiAliasing==render::AntiAliasing::MSAAx4){
            rasterizationShader.shadingSimpling4();
        }else{
            rasterizationShader.shading();
        }

    }

    //片段作色 Gbuffer
    FragmentShader fragmentShader;
    fragmentShader.width=width;
    fragmentShader.height=height;
    fragmentShader.zBuffer=context->zBuffer;
    fragmentShader.gBuffer=context->gBuffer;
    fragmentShader.antiAliasing=context->config.antiAliasing;
    context->gBufferCache.clear();
    fragmentShader.gBufferCache=&context->gBufferCache;

    for(int i=1;i<context->obj.size();i++) {
        auto object = context->obj[i];
        fragmentShader.object=object;
        fragmentShader.shading();
    }

    colorShader.gBufferCache=&context->gBufferCache;
    colorShader.shading();
    //colorShader.shadingZBuffer(context->zBuffer,sampling);




}

void render_core::GraphicsPipeline::prepare() {

    //初始化Z buffer
    if(context->config.antiAliasing==render::AntiAliasing::MSAAx4){
        memset(context->zBuffer,0,context->width*context->height*sizeof(float)*4);
    }else{
        memset(context->zBuffer,0,context->width*context->height*sizeof(float));
    }


    //初始化G buffer
    memset(context->gBuffer,0,sizeof(GBufferUnit)*context->width*context->height);





}

render_core::GraphicsPipeline::~GraphicsPipeline() {

}
