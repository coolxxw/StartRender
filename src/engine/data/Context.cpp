//
// Created by xxw on 2022/11/10.
//

#include "Context.h"
#include "../../include/GltfFile.h"
#include "../shader/VertexShader.h"

using namespace render_core;

Context::Context() {

    width=0;
    height=0;
    zBuffer= nullptr;
    frameBuffer=nullptr;
    scene=new SceneData;
    config.antiAliasing=render::AntiAliasing::None;

}



Context::~Context() {
    for(auto i:obj){
        delete i;
    }
    delete (char*)skyboxTexture.left.data;
    delete (char*)skyboxTexture.right.data;
    delete (char*)skyboxTexture.top.data;
    delete (char*)skyboxTexture.back.data;
    delete (char*)skyboxTexture.bottom.data;
    delete (char*)skyboxTexture.front.data;

    delete zBuffer;
    delete frameBuffer;
    delete gBuffer;
    delete scene;
}



void Context::update(ContextUpdate *contextUpdate, const render::ContextCache *contextCache) {
    if(contextUpdate->updateScene){
        contextUpdate->updateScene= false;

        delete this->scene;
        this->scene=contextUpdate->sceneData;
        contextUpdate->sceneData= nullptr;
        //更新Object
        updateObject();
    }



    //更新 分辨率
    if(width != contextCache->width || height != contextCache->height){
        width=contextCache->width;
        height=contextCache->height;

        delete zBuffer;
        delete frameBuffer;
        delete gBuffer;


        zBuffer=new float [width*height];
        gBuffer=new GBufferUnit [width*height];
        frameBuffer=new RGBA[width * height];

    }
    //相机
    camera=contextCache->camera;
    //光照
    light=contextCache->light;

    if(config.antiAliasing != contextCache->config.antiAliasing){
        delete zBuffer;
        if(contextCache->config.antiAliasing == render::AntiAliasing::MSAAx4){
            zBuffer=new float [width*height*4];
        }else{
            zBuffer=new float [width*height];
        }
    }

    config=contextCache->config;
}

void Context::updateObject() {

    for(auto i:obj){
        delete i;
    }
    this->obj.clear();

    //准备Object List
    if(this->obj.empty()){
        this->obj.resize(1);
    }

    for(int i=1;i<scene->meshes.size();i++) {

        if (i >= this->obj.size()) {

            auto mesh = scene->meshes[i];
            auto vertex = scene->buffers[mesh.vertex];
            auto o = new Object(vertex.length / (sizeof(Vector4f)));
            this->obj.push_back(o);

            o->matrix = mesh.matrix;
            if (mesh.indices == 0) {
                continue;
            }
            o->indicesCount = scene->buffers[mesh.indices].length / 4;
            o->indices = (unsigned int *) scene->buffers[mesh.indices].data;
            if (mesh.vertex == 0) {
                continue;
            }
            if (mesh.uv != 0) {
                o->uvTexture = (float *) scene->buffers[mesh.uv].data;
            }
            if (mesh.normal != 0) {
                o->normal = (Vector3f *) (scene->buffers[mesh.normal].data);
            }
            o->preVertices = (Vector4f *) scene->buffers[mesh.vertex].data;


            auto material = scene->materials[mesh.material];

            //准备法线贴图
            if (material == nullptr || material->normalTexture == 0) {
                o->normalTexture = TextureMap(Texture::defaultNormalTexture());
            } else {
                o->normalTexture = TextureMap(scene->textures[material->normalTexture]);
            }

            //准备颜色贴图
            if (material == nullptr || material->baseColorTexture == 0) {
                o->baseColorTexture = TextureMap(Texture::defaultGrayColorTexture());
            } else {
                o->baseColorTexture = TextureMap(scene->textures[material->baseColorTexture]);
            }

            //准备金属粗糙度贴图
            if (material == nullptr || material->metallicRoughnessTexture == 0) {
                o->metalRoughnessTexture = TextureMap(Texture::defaultBlackColorTexture());
            } else {
                o->metalRoughnessTexture = TextureMap(scene->textures[material->metallicRoughnessTexture]);
            }

            //准备自发光贴图
            if (material == nullptr || material->emissionTexture == 0) {
                o->emissionTexture = TextureMap(Texture::defaultBlackColorTexture());
            } else {
                o->emissionTexture = TextureMap(scene->textures[material->emissionTexture]);
            }

            //计算场景的边界
            float x_min = FLT_MAX, x_max = -FLT_MAX, y_min = FLT_MAX, y_max = -FLT_MAX, z_min = FLT_MAX, z_max = -FLT_MAX;
            for (int o_i = 0; o_i < o->verticesCount; o_i++) {
                if (o->preVertices[o_i].x < x_min) {
                    x_min = o->preVertices[o_i].x;
                }
                if (o->preVertices[o_i].x > x_max) {
                    x_max = o->preVertices[o_i].x;
                }
                if (o->preVertices[o_i].y < y_min) {
                    y_min = o->preVertices[o_i].y;
                }
                if (o->preVertices[o_i].y > y_max) {
                    y_max = o->preVertices[o_i].y;
                }
                if (o->preVertices[o_i].z < z_min) {
                    z_min = o->preVertices[o_i].z;
                }
                if (o->preVertices[o_i].z > z_max) {
                    z_max = o->preVertices[o_i].z;
                }
            }

            auto x = x_max - x_min;
            auto y = x_max - y_min;
            auto z = z_max - z_min;
            o->size = sqrt(x * x + y * y + z * z);
            o->centor = Vector3f((x_max + x_min) / 2, (y_max + y_min) / 2, (z_max + z_min) / 2);
            auto &sceneMin = this->sceneParam.sceneMin;
            auto &sceneMax = this->sceneParam.sceneMax;
            sceneMin.x = fmin(sceneMin.x, x_min);
            sceneMin.y = fmin(sceneMin.y, y_min);
            sceneMin.z = fmin(sceneMin.z, z_min);
            sceneMax.x = fmax(sceneMax.x, x_max);
            sceneMax.y = fmax(sceneMax.y, y_max);
            sceneMax.z = fmax(sceneMax.z, z_max);

            VertexShader::vertexAttributeShading(o->indicesCount, o->indices, o->verticesCount, o->preVertices,
                                                 o->normal, o->uvTexture, o->attribute);
        }
    }
}

void Context::updateSkyboxMap() {
    //准备天空盒

    if(skyboxTexture.left.data){
        this->skybox.left=TextureMap( &this->skyboxTexture.left);
    }else{
        this->skybox.left=TextureMap(Texture::defaultBlackColorTexture());
    }
    if(skyboxTexture.right.data) {
        this->skybox.right = TextureMap(&this->skyboxTexture.right);
    }else{
        this->skybox.right=TextureMap(Texture::defaultBlackColorTexture());
    }
    if(skyboxTexture.top.data) {
        this->skybox.top = TextureMap(&this->skyboxTexture.top);
    }else{
        this->skybox.top=TextureMap(Texture::defaultBlackColorTexture());
    }
    if(skyboxTexture.bottom.data) {
        this->skybox.bottom = TextureMap(&this->skyboxTexture.bottom);
    }else{
        this->skybox.bottom=TextureMap(Texture::defaultBlackColorTexture());
    }
    if(skyboxTexture.front.data) {
        this->skybox.front = TextureMap(&this->skyboxTexture.front);
    }else{
        this->skybox.front=TextureMap(Texture::defaultBlackColorTexture());
    }
    if(skyboxTexture.back.data) {
        this->skybox.back = TextureMap(&this->skyboxTexture.back);
    }else{
        this->skybox.back=TextureMap(Texture::defaultBlackColorTexture());
    }

}

