#include <utility>
#include <cassert>
#include "../engine/data/SceneData.h"
#include "../include/Scene.h"
using render_core::SceneData;
using Buffer = render_core::SceneData::Buffer;
using Texture = render_core::SceneData::Texture;
using Material = render_core::SceneData::Material;
using Mesh = render_core::SceneData::Mesh;

unsigned int render::Scene::addBuffer(const void *data, unsigned int length) {
    assert(this->d!=nullptr);
    if(data== nullptr){
        return 0;
    }

    Buffer buffer;
    buffer.data=new char[length];
    memmove(buffer.data,data,length);
    buffer.length=length;
    this->d->buffers.push_back(buffer);
    return this->d->buffers.size()-1;
}


unsigned int render::Scene::addTextureRGBA(std::string t_name, const void *data, unsigned int w, unsigned int h) {
    assert(this->d!=nullptr);
    if(data== nullptr){
        return 0;
    }

    auto texture=new Texture;
    texture->width=w;
    texture->height=h;
    texture->name=std::move(t_name);
    texture->data=new char[w*h*4];
    memmove(texture->data,data,w*h*4);
    this->d->textures.push_back(texture);
    return this->d->textures.size()-1;
}

render::Scene::MaterialId render::Scene::createMaterial(std::string name) {
    assert(this->d!=nullptr);

    auto material = new Material;
    material->name=std::move(name);
    this->d->materials.push_back(material);
    return this->d->materials.size()-1;
}

render::Scene::MeshId render::Scene::createMesh(std::string name) {
    assert(this->d!=nullptr);

    auto mesh= Mesh();
    mesh.name=std::move(name);
    this->d->meshes.push_back(mesh);
    return this->d->meshes.size()-1;
}


bool render::Scene::bindIndices(render::Scene::MeshId mesh, render::Scene::BufferId indices) {
    assert(this->d!=nullptr);
    if(mesh ==0 || mesh >= this->d->meshes.size()){
        return false;
    }
    if(indices == 0){
        this->d->meshes[mesh].indices=0;
        return true;
    }
    if(indices >= this->d->buffers.size()){
        return false;
    }
    if(this->d->meshes[mesh].vertex){
        if(checkVertex(indices,this->d->meshes[mesh].vertex)){
            this->d->meshes[mesh].indices=indices;
            return true;
        }else{
            return false;
        }
    }
    this->d->meshes[mesh].indices=indices;
    return true;
}

bool render::Scene::bindVertex(render::Scene::MeshId mesh, render::Scene::BufferId vertex) {
    assert(this->d!=nullptr);
    if(mesh==0 || mesh >= this->d->meshes.size()){
        return false;
    }
    if(vertex == 0){
        this->d->meshes[mesh].vertex=0;
        return true;
    }
    if(vertex >= this->d->buffers.size()){
        return false;
    }
    if(this->d->meshes[mesh].indices){
        if(this->d->meshes[mesh].indices,vertex){
            this->d->meshes[mesh].vertex=vertex;
            return true;
        }else{
            return false;
        }
    }
    this->d->meshes[mesh].vertex=vertex;
    return true;
}

bool render::Scene::bindUvTexture(render::Scene::MeshId mesh, render::Scene::BufferId uv) {
    assert(this->d!=nullptr);

    if(mesh == 0 || mesh >= this->d->meshes.size()){
        return false;
    }
    if(uv==0){
        this->d->meshes[mesh].uv=0;
        return true;
    }
    if(uv >= this->d->buffers.size()){
        return false;
    }

    if(this->d->meshes[mesh].vertex){
        if(this->checkUvTexture(this->d->meshes[mesh].vertex,uv)){
            this->d->meshes[mesh].uv=uv;
            return true;
        }else{
            return false;
        }
    }else{
        this->d->meshes[mesh].uv=uv;
        return true;
    }
}

bool render::Scene::bindMaterial(render::Scene::MeshId mesh, render::Scene::MaterialId material) {
    assert(this->d!=nullptr);

    if(mesh == 0 || mesh >= this->d->meshes.size()){
        return false;
    }
    if(material >= this->d->materials.size()){
        return false;
    }

    this->d->meshes[mesh].material=material;
    return true;
}

bool render::Scene::bindNormalTexture(render::Scene::MaterialId material, render::Scene::TextureId texture) {
    assert(this->d!=nullptr);

    if(material==0 || material >= this->d->materials.size()){
        return false;
    }
    if(texture >= this->d->textures.size()){
        return false;
    }
    this->d->materials[material]->normalTexture=texture;

    return true;
}

bool render::Scene::bindBaseColorTexture(render::Scene::MaterialId material, render::Scene::TextureId texture) {
    assert(this->d!=nullptr);

    if(material==0 ||material >= this->d->materials.size()){
        return false;
    }
    if(texture >= this->d->textures.size()){
        return false;
    }
    this->d->materials[material]->baseColorTexture=texture;
    return true;
}

bool render::Scene::bindMetallicRoughnessTexture(render::Scene::MaterialId material, render::Scene::TextureId texture) {
    assert(this->d!=nullptr);

    if(material==0 ||material >= this->d->materials.size()){
        return false;
    }
    if(texture >= this->d->textures.size()){
        return false;
    }
    this->d->materials[material]->metallicRoughnessTexture=texture;
    return true;
}


bool render::Scene::checkUvTexture(render::Scene::BufferId vertex, render::Scene::BufferId uv) {
    auto vBuffer= this->d->buffers[vertex];
    auto uvBuffer= this->d->buffers[uv];
    unsigned int vertexNumber=(vBuffer.length/(4*4));//float x 4
    unsigned int uvNumber=(uvBuffer.length/(4*2));//float x 2
    return vertexNumber <= uvNumber;
}

bool render::Scene::checkVertex(render::Scene::BufferId indices, render::Scene::BufferId vertex) {
    auto vBuffer= this->d->buffers[vertex];
    auto iBuffer= this->d->buffers[indices];
    unsigned int vertexNumber=(vBuffer.length/(4*4));//float x 4
    unsigned int iNumber=(iBuffer.length/(4));//unsigned int
    auto num=(unsigned int*)iBuffer.data;
    for(int i=0;i<iNumber;i++){
        if(num[i]>=vertexNumber){
            return false;
        }
    }
    return true;
}

render::Scene::BufferId render::Scene::addVertexBuffer(const void *data, unsigned int length) {
    assert(this->d!=nullptr);
    if(data== nullptr){
        return 0;
    }

    Buffer buffer;
    unsigned int vertexNumber=length/(4*3);

    buffer.data=new float[vertexNumber*4];
    auto v3=(float*)data;
    auto v4=(float*)buffer.data;
    for(int i=0;i<vertexNumber;i++){
        v4[4*i+0]=v3[3*i+0];
        v4[4*i+1]=v3[3*i+1];
        v4[4*i+2]=v3[3*i+2];
        v4[4*i+3]=1.0f;
    }
    buffer.length=vertexNumber*4*4;
    this->d->buffers.push_back(buffer);
    return this->d->buffers.size()-1;
}

bool render::Scene::bindNormal(render::Scene::MeshId mesh, render::Scene::BufferId normal) {
    assert(this->d!=nullptr);
    if(mesh<=0 || mesh>=d->meshes.size()){
        return false;
    }
    if(normal<=0 ||normal>=d->buffers.size()){
        return false;
    }

    auto vertex= d->meshes[mesh].vertex;
    auto vertexLenght=d->buffers[vertex].length/(4*4);//float*4
    auto normalLenght=d->buffers[normal].length/(4*3);
    if(normalLenght<vertexLenght){
        return false;
    }
    d->meshes[mesh].normal=normal;
    return true;
}
