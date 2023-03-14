//
// Created by xxw on 2023/3/8.
//

#include "GraphicsPipeline.h"
#include "shader/VertexShader.h"
#include "shader/ColorShader.h"
#include "shader/FragmentShader.h"

void render_core::GraphicsPipeline::render() {
    prepare();

    int width=context->width;
    int height=context->height;

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
        //matrix.move(-context->object[0]->center);
        //float scale=1/context->object[0]->objectSize;
        //matrix.scale(Vector3f{scale,scale,scale});
        if(context->obj.size()>=2){
            float scale=1/context->obj[1]->size;
           matrix.move(-context->obj[1]->centor);
            matrix.scale(Vector3f(scale,scale,scale));
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
    for(int i=1;i<context->obj.size();i++){
        auto object=context->obj[i];
        RasterizationShader::shading(width,height,context->zBuffer,object->indices,object->indicesCount,object->vertices);
    }

    //Gbuffer
    for(int i=1;i<context->obj.size();i++) {
        auto object = context->obj[i];
        FragmentShader::shading(width, height, context->zBuffer, object->indices, object->indicesCount,
                                object->vertices,object->preVertices,object->attribute,object->normalTexture,object->baseColorTexture,gBuffer);
    }

    ColorShader::shading(width,height,light,this->gBuffer,context->frameBuffer);




}

void render_core::GraphicsPipeline::prepare() {

   //初始化framebuffer
    {
        RGBA* frameBuffer=context->frameBuffer;
       // auto color=RGBA(127, 100, 50, 255);
       // memset(frameBuffer,127,4*context->width*context->height);
       // for(int i=0;i<(context->width*context->height);i++)
        //{
       //     //设置屏幕背景色
       //     frameBuffer[i]=RGBA(127, 127, 127, 255);
       // }
    }

    //初始化Z buffer
    memset(context->zBuffer,0,context->width*context->height*sizeof(float));
    //for(int i=0;i<(context->width*context->height);i++)
   // {
    //    context->zBuffer[i]=0;//
    //}

    //初始化G buffer
    if(context->width*context->height!=0){
        if(context->width!=gBufferWidth || context->height!=gBufferHeight){
            delete[] gBuffer;
            gBuffer=new GBufferUnit[context->height*context->width];
            gBufferWidth=context->width;
            gBufferHeight=context->height;
        }
        memset(gBuffer,0,sizeof(GBufferUnit)*gBufferWidth*gBufferHeight);
    }


    //准备Object List
    if(context->obj.size()==0){
        context->obj.resize(1);
    }
    auto scene=context->scene;
    for(int i=1;i<scene->meshes.size();i++){
        if(i>=context->obj.size()){

            auto mesh=scene->meshes[i];
            auto vertex= scene->buffers[mesh.vertex];
            auto o=new Object(vertex.length/(sizeof(Vector4f)));
            context->obj.push_back(o);
            if(mesh.indices==0){
                continue;
            }
            o->indicesCount=scene->buffers[mesh.indices].length/4;
            o->indices= (unsigned int*)scene->buffers[mesh.indices].data;
            if(mesh.vertex==0){
                continue;
            }
            if(mesh.uv!=0){
                o->uvTexture=(float*)scene->buffers[mesh.uv].data;
            }
            if(mesh.normal!=0){
                o->normal= reinterpret_cast<Vector3f *>((float *) scene->buffers[mesh.normal].data);
            }
            o->preVertices=(Vector4f*)scene->buffers[mesh.vertex].data;

            static RGBA defaultTexture[1*1]={RGBA{127,127,127,0}};
            auto material=scene->materials[mesh.material];

            if( material ==nullptr || material->normalTexture==0){
                o->normalTexture.width =1;
                o->normalTexture.height =1;
                o->normalTexture.texture = reinterpret_cast<const RGBA *>(&defaultTexture);
            }else{
                o->normalTexture.width =scene->textures [material->normalTexture]->width;
                o->normalTexture.height =scene->textures [material->normalTexture]->height;
                o->normalTexture.texture =(RGBA*)scene->textures [material->normalTexture]->data;
            }


            if(material ==nullptr ||material->baseColorTexture==0){
                o->baseColorTexture.width =1;
                o->baseColorTexture.height =1;
                o->baseColorTexture.texture = reinterpret_cast<const RGBA *>(&defaultTexture);
            }else{
                o->baseColorTexture.width =scene->textures [material->baseColorTexture]->width;
                o->baseColorTexture.height =scene->textures [material->baseColorTexture]->height;
                o->baseColorTexture.texture =(RGBA*)scene->textures [material->baseColorTexture]->data;
            }

            float x_min=FLT_MAX,x_max=-FLT_MAX,y_min=FLT_MAX,y_max=-FLT_MAX,z_min=FLT_MAX,z_max=-FLT_MAX;
            for (i=0;i<o->verticesCount;i++){
                if (o->preVertices[i].x<x_min){
                    x_min=o->preVertices[i].x;
                }
                if(o->preVertices[i].x>x_max){
                    x_max=o->preVertices[i].x;
                }
                if (o->preVertices[i].y<y_min){
                    y_min=o->preVertices[i].y;
                }
                if(o->preVertices[i].y>y_max){
                    y_max=o->preVertices[i].y;
                }
                if (o->preVertices[i].z<z_min){
                    z_min=o->preVertices[i].z;
                }
                if(o->preVertices[i].z>z_max){
                    z_max=o->preVertices[i].z;
                }
            }
            auto x=x_max-x_min;
            auto y=x_max-y_min;
            auto z=z_max-z_min;
            o->size=sqrt(x*x+y*y+z*z);
            o->centor=Vector3f((x_max+x_min)/2,(y_max+y_min)/2,(z_max+z_min)/2);


            VertexShader::vertexAttributeShading(o->indicesCount,o->indices,o->verticesCount,o->preVertices,o->normal,o->uvTexture,o->attribute);
        }
    }
}

render_core::GraphicsPipeline::~GraphicsPipeline() {
    delete[] gBuffer;
}
