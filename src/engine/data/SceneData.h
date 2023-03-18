//
// Created by xxw on 2023/3/8.
//

#ifndef STARTRENDER_SCENEDATA_H
#define STARTRENDER_SCENEDATA_H
#include <string>
#include <vector>

#include "../../math/Matrix.h"

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
        };

        struct Texture{
            std::string name;
            unsigned int width=0;
            unsigned int height=0;
            void* data=nullptr;//RGBA 表示
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

        struct Skybox{
            //索引Texture
            unsigned int top=0;
            unsigned int left=0;
            unsigned int right=0;
            unsigned int bottom=0;
            unsigned int front=0;
            unsigned int back=0;
        };

    public:
        std::string name;//场景名
        std::vector<Buffer> buffers;
        std::vector<Texture*> textures;
        std::vector<Material*> materials;
        std::vector<Mesh> meshes;
        Skybox skybox;

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
