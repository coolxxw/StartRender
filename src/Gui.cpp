//
// Created by xxw on 2023/2/20.
#include <fstream>


//引入SDL+Imgui
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"
#include <cstdio>
#include <SDL.h>
#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif



#include "Gui.h"
#include "./include/Util.h"
#include "include/GltfFile.h"


class GuiData{
public:
    SDL_Texture *sdl_texture= nullptr;
    SDL_Renderer* renderer;
};

Gui::Gui() {
    d=new GuiData;
    render=new render::StartRender(0, 0);
}

void Gui::exec() {

    //以下部分照搬官方源码
// Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return ;
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("StartRender", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 1024, window_flags);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,  SDL_RENDERER_SOFTWARE);
    if (renderer == NULL)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return ;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;


    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);


    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;


    //加载汉字字符表
    std::fstream file;
    ImVector<ImWchar> chineseRange;
    ImFontGlyphRangesBuilder chineseGlyph;
    file.open("assets/chinese.txt",std::ios_base::in);
    if(file.is_open()){
        while(!file.eof()){
            std::string s;
            file>>s;
            chineseGlyph.AddText(s.c_str());
        }
    }
    //添加ASCII
    for(int i=0x20;i<=0xFF;i++){
        char e[]={(char)i,0};
        chineseGlyph.AddText(e);
    }
    chineseGlyph.BuildRanges(&chineseRange);
    //加载字体 Windows 微软雅黑
    ImFont* font=io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/simhei.ttf",18.0f,NULL,chineseRange.Data);
    //io.Fonts->AddFontFromFileTTF("assets/fonts/LXGWWenKai-Regular.ttf",18.0f,NULL,chineseRange.Data);
    //完成加载汉字字符表

    //设置下渲染引擎
    render->registerPaintImpl(this);
    auto scene=render->getScene();
    //加载天空盒
    scene.addSkyBoxImage("./assets/skybox/front.jpg",render::Scene::SkyBoxDirection::Front);
    scene.addSkyBoxImage("./assets/skybox/back.jpg",render::Scene::SkyBoxDirection::Back);
    scene.addSkyBoxImage("./assets/skybox/top.jpg",render::Scene::SkyBoxDirection::Top);
    scene.addSkyBoxImage("./assets/skybox/bottom.jpg",render::Scene::SkyBoxDirection::Bottom);
    scene.addSkyBoxImage("./assets/skybox/left.jpg",render::Scene::SkyBoxDirection::Left);
    scene.addSkyBoxImage("./assets/skybox/right.jpg",render::Scene::SkyBoxDirection::Right);
    //加载模型
    //GltfFile *gltfFile=new GltfFile("D:/IT/git/glTF-Sample-Models-master/2.0/BoomBoxWithAxes/glTF/BoomBoxWithAxes.gltf");
    //GltfFile *gltfFile=new GltfFile("D:/IT/git/glTF-Sample-Models-master/2.0/MetalRoughSpheres/glTF/MetalRoughSpheres.gltf");
   // GltfFile *gltfFile=new GltfFile("bee.glb");
   // GltfFile *gltfFile=new GltfFile("D:/IT/git/glTF-Sample-Models-master/2.0/NormalTangentTest/glTF/NormalTangentTest.gltf");
     GltfFile *gltfFile=new GltfFile(R"(D:\IT\git\glTF-Sample-Models-master\2.0\DamagedHelmet\glTF\DamagedHelmet.gltf)");
    gltfFile->fillScene(&scene);
    delete gltfFile;
    render->start();
    this->d->renderer=renderer;



    const unsigned int FPS=1000/60;//20可替换为限制的帧速
    Uint32 _FPS_Timer=0;

    // Main loop
    while (!done) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }
        //限制帧率
        if(SDL_GetTicks()-_FPS_Timer<FPS){
            SDL_Delay(FPS-SDL_GetTicks()+_FPS_Timer);
        }
        _FPS_Timer=SDL_GetTicks();

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();



        //以下部分是Gui控件代码
        int display_w =ImGui::GetIO().DisplaySize.x;
        int display_h=ImGui::GetIO().DisplaySize.y;

        render->setSize(display_w,display_h);



        if(ImGui::IsKeyPressed(ImGuiKey_F1)){
            this->showController=!this->showController;
        }



        if (showController) {
            ImGui::Begin("Controller");
            ImGui::Text("3D渲染引擎");
            ImGui::Text("渲染分辨率: %dx%d", display_w, display_h);
            ImGui::Text("渲染帧率: %.1f FPS  %lld", render->getFps(), render->getFrameCounter());
            ImGui::Text("输出帧率: %.1f FPS", ImGui::GetIO().Framerate);

            if (ImGui::TreeNode("光照")) {

                ImGui::SeparatorText("默认直射光");
                static float knobh = 0, knobv = 0;
                //render::Util::VectorToAngle(lightParam.directionalLight, &knobh, &knobv);
                ImGui::SliderFloat("水平方向", &knobh, 0, 360);
                ImGui::SliderFloat("垂直方向", &knobv, -90, 90);
                //lightParam.directionalLight = render::Util::AngleToVector(knobh, knobv);

                ImGui::SeparatorText("漫反射");
                //ImGui::SliderFloat("漫反射系数", &lightParam.diffuse.a, 0, 1.0f);
                ImGui::SeparatorText("镜面反射");
                //ImGui::SliderFloat("镜反射系数", &lightParam.specular.a, 0, 1.0f);
                //ImGui::SliderInt("指数", &lightParam.specularExponent, 1, 256);
                ImGui::SeparatorText("环境光");
                //ImGui::SliderFloat("环境光系数", &lightParam.ambient.a, 0, 1.0f);
                ImGui::TreePop();
            }

            ImGui::End();
        } else {

            if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                render::Camera camera = render->getCamera();
                camera.rotateAroundCenter(io.MouseDelta.x / 200, io.MouseDelta.y / 200);
            }


            if (io.MouseWheel) {
                render::Camera camera = render->getCamera();
                camera.moveForward(io.MouseWheel / 100);
            }

        }//if showController结束

        //结束Gui控件代码



        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_RenderClear(renderer);


        frameBufferLock.outLock();
        if(frameBuffer_w!=display_w || frameBuffer_h!=display_h){
            SDL_DestroyTexture(d->sdl_texture);
            d->sdl_texture=SDL_CreateTexture(d->renderer,SDL_PIXELFORMAT_RGBA32,SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET ,display_w,display_h);
        }
        if(this->frameBuffer_w==display_w && this->frameBuffer_h==display_h){
            SDL_Rect  rect={0,0,frameBuffer_w,frameBuffer_h};
            SDL_UpdateTexture(d->sdl_texture,&rect,frameBuffer,4*frameBuffer_w);
            SDL_RenderCopyEx(renderer,this->d->sdl_texture,&rect,&rect,0,0,SDL_FLIP_NONE);
        }
        frameBufferLock.outUnlock();

        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);


    }//while


    // Cleanup
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}


Gui::~Gui() {
    delete render;
    delete d;
    delete[] (char*)frameBuffer;
}

void Gui::paint(void *buffer, int width, int height) {
    if(width*height==0){
        return;
    }
    frameBufferLock.inLock();
    if(frameBuffer_w!=width || frameBuffer_h!=height){
        delete frameBuffer;
       frameBuffer=new char[width*height*4];
    }
    memmove(frameBuffer,buffer,width*height*4);

    frameBuffer_h=height;
    frameBuffer_w=width;

    frameBufferLock.inUnlock();
}


void Gui::mousePosCallback(double x, double y) {
    if(mouseMoveFlag){
        double a=x-mouseMoveX;
        double b=y-mouseMoveY;
        mouseMoveX=x;
        mouseMoveY=y;

        printf("mouse\n");
        render::Camera camera=render->getCamera();
        camera.rotateAroundCenter(a/100,b/100);

    }else{
        mouseMoveX=x;
        mouseMoveY=y;
    }
}
