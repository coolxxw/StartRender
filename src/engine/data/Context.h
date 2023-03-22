//
// Created by xxw on 2022/11/10.
//

#ifndef STARTRENDER_CONTEXT_H
#define STARTRENDER_CONTEXT_H

#include "../../include/Camera.h"
#include "../../include/Environment.h"
#include "../Viewing.h"
#include "../../include/type.h"
#include "../Util.h"
#include "Object.h"
#include "SceneData.h"
#include "CubeMap.h"
#include "../../include/ContextCache.h"
#include "GBuffer.h"

//渲染上下文

namespace render_core{

    struct SkyBox{
        Texture right;
        Texture top;
        Texture bottom;
        Texture front;
        Texture back;

    public:
//索引Texture
Texture left;
    };

    class ContextUpdate{
    public:
        SceneData* sceneData=nullptr;
        bool updateScene=false;
    };

    class SceneParam{
    public:
        //记录场景中物体的大小
        Vector3f sceneMin=Vector3f{0,0,0};
        Vector3f sceneMax=Vector3f{0,0,0};
    };




    //渲染上下文
    class Context {
    public:
        int width;//帧缓冲宽度
        int height;//帧缓冲高度
        RGBA* frameBuffer;//帧缓冲
        Camera camera;//相机
        render::Environment light;
        SceneData *scene;
        std::vector<Object*> obj;
        SceneParam sceneParam;
        SkyBox skyboxTexture;
        CubeMap skybox;
        ClipSpace clipSpace;
        //如果开启MSAA 大小为MSAA倍数
        float * zBuffer;
        GBufferUnit* gBuffer=nullptr;
        GBufferCache gBufferCache;
        render::Config config{};



    public:
        Context();
        ~Context();

        void update(ContextUpdate* contextUpdate,const render::ContextCache* contextCache);

        void updateObject();
        void updateSkyboxMap();

    };

}



#endif //STARTRENDER_CONTEXT_H
