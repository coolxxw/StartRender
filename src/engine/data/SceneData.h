//
// Created by xxw on 2023/3/8.
//

#ifndef STARTRENDER_SCENEDATA_H
#define STARTRENDER_SCENEDATA_H
#include <string>
#include <vector>

#include "../../math/Matrix.h"
#include "Texture.h"

namespace render_core{

    class SceneData {
    public:

        //类型定义
        struct Buffer{
            unsigned int length=0;
            void* data=nullptr;
        };

        struct Material{
            std::string name;
            unsigned int normalTexture=0;//texture id
            unsigned int baseColorTexture=0;//texture id
            unsigned int metallicRoughnessTexture=0;//texture id
            unsigned int emissionTexture=0;
        };




        struct Mesh{
            std::string name;
            unsigned int indices=0;//buffers id
            unsigned int vertex=0; //buffers id
            unsigned int material=0;
            unsigned int normal=0;
            unsigned int uv=0;
            Matrix4f matrix;
        };


    public:
        std::string name;//场景名
        std::vector<Buffer> buffers;
        std::vector<Texture*> textures;
        std::vector<Material*> materials;
        std::vector<Mesh> meshes;

        SceneData(){
            buffers.resize(1);
            textures.resize(1);
            materials.resize(1);
            meshes.resize(1);
        }

        ~SceneData(){
            for(auto i:buffers){
                delete [] (char*)i.data;
            }
            buffers.clear();

            for(auto i:textures){
                if(i){
                    delete[] (char*)i->data;
                    delete i;
                }
            }
            textures.clear();

            for(auto i:materials){
                delete i;
            }
            materials.clear();
        };

    };
}

#endif //STARTRENDER_SCENEDATA_H
