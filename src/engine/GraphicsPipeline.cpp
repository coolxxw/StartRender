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

    ColorShader colorShader;
    colorShader.width=width;
    colorShader.height=height;
    colorShader.framebuffer=context->frameBuffer;
    colorShader.gBuffer=gBuffer;
    colorShader.lightParam=&context->light;
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
            Vector3f v=this->sceneMax-this->sceneMin;
            Vector3f centor=(this->sceneMax+this->sceneMin)*0.5;
            float size= sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
            {
                float scale=1.5/size;
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
    for(int i=1;i<context->obj.size();i++){
        auto object=context->obj[i];
        RasterizationShader::shading(width,height,context->zBuffer,object->indices,object->indicesCount,object->vertices);
    }

    //Gbuffer
    for(int i=1;i<context->obj.size();i++) {
        auto object = context->obj[i];
        FragmentShader::shading(width, height, context->zBuffer, object->indices, object->indicesCount,
                                object->vertices,object->preVertices,object->attribute,
                                object->normalTexture,
                                object->baseColorTexture,
                                object->metalRoughnessTexture,
                                gBuffer);
    }

    colorShader.shading();




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

    //准备天空盒
    if(context->scene->skybox.back){
        context->skybox.back=TextureMap( context->scene->textures[context->scene->skybox.back]);
    }
    //准备天空盒
    if(context->scene->skybox.left){
        context->skybox.left=TextureMap( context->scene->textures[context->scene->skybox.left]);
    }
    //准备天空盒
    if(context->scene->skybox.right){
        context->skybox.right=TextureMap( context->scene->textures[context->scene->skybox.right]);
    }
    //准备天空盒
    if(context->scene->skybox.top){
        context->skybox.top=TextureMap( context->scene->textures[context->scene->skybox.top]);
    }
    //准备天空盒
    if(context->scene->skybox.bottom){
        context->skybox.bottom=TextureMap( context->scene->textures[context->scene->skybox.bottom]);
    }
    //准备天空盒
    if(context->scene->skybox.front){
        context->skybox.front=TextureMap( context->scene->textures[context->scene->skybox.front]);
    }


    //准备Object List
    if(context->obj.empty()){
        context->obj.resize(1);
    }
    auto scene=context->scene;
    for(int i=1;i<scene->meshes.size();i++){

        if(i>=context->obj.size()){

            auto mesh=scene->meshes[i];
            auto vertex= scene->buffers[mesh.vertex];
            auto o=new Object(vertex.length/(sizeof(Vector4f)));
            context->obj.push_back(o);

            o->matrix=mesh.matrix;
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
                o->normal= (Vector3f *)( scene->buffers[mesh.normal].data);
            }
            o->preVertices=(Vector4f*)scene->buffers[mesh.vertex].data;

            static RGBA defaultTexture[1*1]={RGBA(127,127,127)};
            auto material=scene->materials[mesh.material];

            if( material ==nullptr || material->normalTexture==0){
                static RGBA defaultNormalTexture[2*2]={RGBA{127,127,255,0},RGBA{127,127,255,0},RGBA{127,127,255,0},RGBA{127,127,255,0}};
                o->normalTexture.width =1;
                o->normalTexture.height =1;
                o->normalTexture.texture = reinterpret_cast<const RGBA *>(&defaultNormalTexture);
            }else{
                o->normalTexture.width =scene->textures [material->normalTexture]->width;
                o->normalTexture.height =scene->textures [material->normalTexture]->height;
                o->normalTexture.texture =(RGBA*)scene->textures [material->normalTexture]->data;
            }


            if(material ==nullptr ||material->baseColorTexture==0){
                static RGBA defaultColorTexture[2*2]={RGBA{127,127,127,0},RGBA{127,127,127,0},RGBA{127,127,127,0},RGBA{127,127,127,0}};
                o->baseColorTexture.width =1;
                o->baseColorTexture.height =1;
                o->baseColorTexture.texture = reinterpret_cast<const RGBA *>(&defaultColorTexture);
            }else{
                o->baseColorTexture.width =scene->textures [material->baseColorTexture]->width;
                o->baseColorTexture.height =scene->textures [material->baseColorTexture]->height;
                o->baseColorTexture.texture =(RGBA*)scene->textures [material->baseColorTexture]->data;
            }

            if(material ==nullptr ||material->metallicRoughnessTexture==0){
                o->metalRoughnessTexture.width =1;
                o->metalRoughnessTexture.height =1;
                o->metalRoughnessTexture.texture = reinterpret_cast<const RGBA *>(&defaultTexture);
            }else{
                o->metalRoughnessTexture.width =scene->textures [material->metallicRoughnessTexture]->width;
                o->metalRoughnessTexture.height =scene->textures [material->metallicRoughnessTexture]->height;
                o->metalRoughnessTexture.texture =(RGBA*)scene->textures [material->metallicRoughnessTexture]->data;
            }

            float x_min=FLT_MAX,x_max=-FLT_MAX,y_min=FLT_MAX,y_max=-FLT_MAX,z_min=FLT_MAX,z_max=-FLT_MAX;
            for (int o_i=0; o_i < o->verticesCount; o_i++){
                if (o->preVertices[o_i].x < x_min){
                    x_min=o->preVertices[o_i].x;
                }
                if(o->preVertices[o_i].x > x_max){
                    x_max=o->preVertices[o_i].x;
                }
                if (o->preVertices[o_i].y < y_min){
                    y_min=o->preVertices[o_i].y;
                }
                if(o->preVertices[o_i].y > y_max){
                    y_max=o->preVertices[o_i].y;
                }
                if (o->preVertices[o_i].z < z_min){
                    z_min=o->preVertices[o_i].z;
                }
                if(o->preVertices[o_i].z > z_max){
                    z_max=o->preVertices[o_i].z;
                }
            }
            auto x=x_max-x_min;
            auto y=x_max-y_min;
            auto z=z_max-z_min;
            o->size=sqrt(x*x+y*y+z*z);
            o->centor=Vector3f((x_max+x_min)/2,(y_max+y_min)/2,(z_max+z_min)/2);
            this->sceneMin.x=fmin(this->sceneMin.x,x_min);
            this->sceneMin.y=fmin(this->sceneMin.y,y_min);
            this->sceneMin.z=fmin(this->sceneMin.z,z_min);
            this->sceneMax.x=fmax(this->sceneMax.x,x_max);
            this->sceneMax.y=fmax(this->sceneMax.y,y_max);
            this->sceneMax.z=fmax(this->sceneMax.z,z_max);

            VertexShader::vertexAttributeShading(o->indicesCount,o->indices,o->verticesCount,o->preVertices,o->normal,o->uvTexture,o->attribute);
        }
    }
}

render_core::GraphicsPipeline::~GraphicsPipeline() {
    delete[] gBuffer;
}
