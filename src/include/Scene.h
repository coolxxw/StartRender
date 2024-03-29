//
// Created by xxw on 2023/3/7.
//

#ifndef STARTRENDER_SCENE_H
#define STARTRENDER_SCENE_H

#include <string>
#include <vector>
#include "../math/Matrix.h"

namespace render_core{
    class SceneData;
}

namespace render{
    //管理一个场景数据资源
    class Scene {

    public:


        typedef unsigned int TextureId;
        typedef unsigned int BufferId;
        typedef unsigned int MaterialId;
        typedef unsigned int MeshId;



    private:

        render_core::SceneData* d= nullptr;

        bool checkUvTexture(BufferId vertex,BufferId uv);

        bool checkVertex(BufferId indices,BufferId vertex);

    public:
        explicit Scene();
        ~Scene();

        render_core::SceneData* moveSceneData();

        TextureId addTextureRGBA(std::string name,const void* data, unsigned int w, unsigned int h);

        /***
         * 添加二进制数据,如果data为nullptr 返回0
         * @param data 源
         * @param length data字节数
         * @return
         */
        BufferId addBuffer(const void* data,unsigned int length);

        /***
         * 添加顶点数据 顶点用FLOAT VEC3表示
         * @param data
         * @param length
         * @return
         */
        BufferId addVertexBuffer(const void* data,unsigned int length);

        /***
         * 创建材质,不包含任何贴图
         * @param name
         * @return
         */
        MaterialId createMaterial(std::string name);

        MeshId createMesh(std::string name);

        /**
         * 为Mesh绑定indices 以unsigned int解析indices指向二进制数据
         * 如果已绑定Vertex则 indices不能超出Vertex范围
         * @param mesh
         * @param indices
         * @return
         */
        bool bindIndices(MeshId mesh,BufferId indices);

        bool bindVertex(MeshId mesh, BufferId vertex);

        bool bindNormal(MeshId mesh, BufferId norrmal);

        bool bindMaterial(MeshId mesh,MaterialId material);

        bool bindUvTexture(MeshId mesh,BufferId uv);

        bool bindNormalTexture(MaterialId material,TextureId texture);

        bool bindBaseColorTexture(MaterialId material,TextureId texture);

        bool bindMetallicRoughnessTexture(MaterialId material,TextureId texture);

        bool translationMesh(MeshId meshId,Matrix4f matrix);

        bool bindEmissionTexture(MaterialId material, TextureId emission);
    };
}




#endif //STARTRENDER_SCENE_H
