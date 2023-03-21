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
            Vector3f v=this->sceneMax-this->sceneMin;
            Vector3f centor=(this->sceneMax+this->sceneMin)*0.5;
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


            auto material=scene->materials[mesh.material];

            //准备法线贴图
            if( material ==nullptr || material->normalTexture==0){
                o->normalTexture=TextureMap(Texture::defaultNormalTexture());
            }else{
                o->normalTexture =TextureMap(scene->textures [material->normalTexture]);
            }

            //准备颜色贴图
            if(material ==nullptr ||material->baseColorTexture==0){
                o->baseColorTexture=TextureMap(Texture::defaultGrayColorTexture());
            }else{
                o->baseColorTexture =TextureMap(scene->textures [material->baseColorTexture]);
            }

            //准备金属粗糙度贴图
            if(material ==nullptr ||material->metallicRoughnessTexture==0){
                o->metalRoughnessTexture=TextureMap(Texture::defaultBlackColorTexture());
            }else{
                o->metalRoughnessTexture=TextureMap(scene->textures [material->metallicRoughnessTexture]);
            }

            //准备自发光贴图
            if(material ==nullptr ||material->emissionTexture==0){
                o->emissionTexture=TextureMap(Texture::defaultBlackColorTexture());
            }else{
                o->emissionTexture=TextureMap(scene->textures [material->emissionTexture]);
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

}
