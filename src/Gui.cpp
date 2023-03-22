#include <fstream>


//引入SDL+Imgui
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"
#include <cstdio>
#include <SDL.h>
#include <sstream>

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif



#include "Gui.h"
#include "include/GltfFile.h"
#include "front.jpg.h"
#include "back.jpg.h"
#include "left.jpg.h"
#include "right.jpg.h"
#include "top.jpg.h"
#include "bottom.jpg.h"
#include "chinese.txt.h"


class GuiData{
public:
    SDL_Texture *sdl_texture= nullptr;
    SDL_Renderer* renderer= nullptr;
};

Gui::Gui() {
    d=new GuiData;
    render=new render::StartRender();
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
    auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("StartRender", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 512, window_flags);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,  SDL_RENDERER_SOFTWARE);
    if (renderer == nullptr)
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


    //加载汉字字符表
    std::fstream file;
    ImVector<ImWchar> chineseRange;
    ImFontGlyphRangesBuilder chineseGlyph;
    /*
    file.open("assets/chinese.txt",std::ios_base::in);
    if(file.is_open()){
        while(!file.eof()){
            std::string s;
            file>>s;
            chineseGlyph.AddText(s.c_str());
        }
    }
     */

    chineseGlyph.AddText(reinterpret_cast<const char *>(R_CHINESE_TXT_DATA));

    //添加ASCII
    for(int i=0x20;i<=0xFF;i++){
        char e[]={(char)i,0};
        chineseGlyph.AddText(e);
    }
    chineseGlyph.BuildRanges(&chineseRange);

    //加载字体 Windows 微软雅黑
    ImFont* font=ImGui::GetIO().Fonts->AddFontFromFileTTF("C:/Windows/Fonts/simhei.ttf",18.0f,NULL,chineseRange.Data);
    if(!font){
        ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/fonts/LXGWWenKai-Regular.ttf",18.0f,NULL,chineseRange.Data);
    }
    //完成加载汉字字符表





    // Main loop
    bool done = false;

    //设置下渲染引擎
    render->registerPaintImpl(this);
    //加载配置文件 加载模型文件
    loadProfile();

    //启动渲染引擎
    render->start();
    this->d->renderer=renderer;


    const unsigned int FPS=1000/60;//20可替换为限制的帧速
    Uint32 FPS_Timer=0;
    float guiAlpha=0.5;//gui透明度

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
            if (event.type==SDL_DROPFILE){
                const char *file=event.drop.file;
                if(file!= nullptr){
                    profile.file=file;
                    loadFile(file);
                }
            }
        }
        //限制帧率
        if(SDL_GetTicks()-FPS_Timer<FPS){
            SDL_Delay(FPS-SDL_GetTicks()+FPS_Timer);
        }
        FPS_Timer=SDL_GetTicks();

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();



        //以下部分是Gui控件代码

        //设置透明度
        ImGui::SetNextWindowBgAlpha(guiAlpha);
        //获取窗口大小
        int display_w =(int)ImGui::GetIO().DisplaySize.x;
        int display_h=(int)ImGui::GetIO().DisplaySize.y;
        render->setSize(display_w,display_h);


        //检测按键
        if(ImGui::IsKeyPressed(ImGuiKey_F1)){
            this->showController=!this->showController;
        }
        //if(ImGui::)



        if (showController) {
            ImGui::Begin("Controller");
            ImGui::Text("3D渲染引擎");
            ImGui::Text("渲染分辨率: %dx%d", display_w, display_h);
            ImGui::Text("渲染帧率: %.1f FPS  %lld", render->getFps(), render->getFrameCounter());
            ImGui::Text("输出帧率: %.1f FPS", ImGui::GetIO().Framerate);

            auto context= render->getContext();
            auto light=&context->light;

            if (ImGui::TreeNode("GUI")) {
                ImGui::SliderFloat("GUI透明", &guiAlpha, 0.0f, 1.0f);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("光照")) {
                if(ImGui::TreeNodeEx("直射光", ImGuiTreeNodeFlags_Selected)){
                    if(ImGui::TreeNodeEx("方向", ImGuiTreeNodeFlags_Selected)){
                        static float knobh = 0, knobv = 0;
                        //render::Util::VectorToAngle(lightParam.directLightVector, &knobh, &knobv);
                        ImGui::SliderFloat("水平方向", &knobh, 0, 360);
                        ImGui::SliderFloat("垂直方向", &knobv, -90, 90);
                        //lightParam.directLightVector = render::Util::AngleToVector(knobh, knobv);
                        ImGui::TreePop();
                    }
                    if(ImGui::TreeNodeEx("颜色", ImGuiTreeNodeFlags_Selected)){
                        ImGui::SliderFloat("R ##directLightColor", &light->directLightColor.r, 0.0f, 1.0f);
                        ImGui::SliderFloat("G ##directLightColor", &light->directLightColor.g, 0.0f, 1.0f);
                        ImGui::SliderFloat("B ##directLightColor", &light->directLightColor.b, 0.0f, 1.0f);
                        ImGui::SliderFloat("A ##directLightColor", &light->directLightColor.a, 0.0f, 1.0f);
                        ImGui::TreePop();
                    }
                    if(ImGui::TreeNodeEx("漫反射", ImGuiTreeNodeFlags_Selected)){
                        ImGui::SliderFloat("R ## directDiffuse", &light->directLightDiffuseFact.r, 0.0f, 1.0f);
                        ImGui::SliderFloat("G ## directDiffuse", &light->directLightDiffuseFact.g, 0.0f, 1.0f);
                        ImGui::SliderFloat("B ## directDiffuse", &light->directLightDiffuseFact.b, 0.0f, 1.0f);
                        ImGui::SliderFloat("F ## directDiffuse", &light->directLightDiffuseFact.a, 0.0f, 1.0f);
                        ImGui::TreePop();
                    }
                    if(ImGui::TreeNodeEx("镜面反射", ImGuiTreeNodeFlags_Selected)){
                        ImGui::SliderFloat("R ## directSpecular", &light->directLightSpecularFact.r, 0.0f, 1.0f);
                        ImGui::SliderFloat("G ## directSpecular", &light->directLightSpecularFact.g, 0.0f, 1.0f);
                        ImGui::SliderFloat("B ## directSpecular", &light->directLightSpecularFact.b, 0.0f, 1.0f);
                        ImGui::SliderFloat("F ## directSpecular", &light->directLightSpecularFact.a, 0.0f, 1.0f);
                        ImGui::TreePop();
                    }
                    ImGui::TreePop();
                }
                if(ImGui::TreeNodeEx("非直射光", ImGuiTreeNodeFlags_Selected)){
                    if(ImGui::TreeNodeEx("漫反射", ImGuiTreeNodeFlags_Selected)){
                        ImGui::SliderFloat("R ## indirectDiffuse", &light->indirectLightDiffuseFact.r, 0.0f, 1.0f);
                        ImGui::SliderFloat("G ## indirectDiffuse", &light->indirectLightDiffuseFact.g, 0.0f, 1.0f);
                        ImGui::SliderFloat("B ## indirectDiffuse", &light->indirectLightDiffuseFact.b, 0.0f, 1.0f);
                        ImGui::SliderFloat("F ## indirectDiffuse", &light->indirectLightDiffuseFact.a, 0.0f, 1.0f);
                        ImGui::TreePop();
                    }
                    if(ImGui::TreeNodeEx("镜面反射", ImGuiTreeNodeFlags_Selected)){
                        ImGui::SliderFloat("R ## indirectSpecular", &light->indirectLightSpecularFact.r, 0.0f, 1.0f);
                        ImGui::SliderFloat("G ## indirectSpecular", &light->indirectLightSpecularFact.g, 0.0f, 1.0f);
                        ImGui::SliderFloat("B ## indirectSpecular", &light->indirectLightSpecularFact.b, 0.0f, 1.0f);
                        ImGui::SliderFloat("F ## indirectSpecular", &light->indirectLightSpecularFact.a, 0.0f, 1.0f);
                        ImGui::TreePop();
                    }
                    ImGui::TreePop();
                }
                if(ImGui::TreeNodeEx("自发光", ImGuiTreeNodeFlags_Selected)){
                    ImGui::SliderFloat("R ## emission", &light->emissionLightFact.r, 0.0f, 1.0f);
                    ImGui::SliderFloat("G ## emission", &light->emissionLightFact.g, 0.0f, 1.0f);
                    ImGui::SliderFloat("B ## emission", &light->emissionLightFact.b, 0.0f, 1.0f);
                    ImGui::SliderFloat("F ## emission", &light->emissionLightFact.a, 0.0f, 2.0f);
                    ImGui::TreePop();
                }
                ImGui::TreePop();//光照
            }

            if(ImGui::TreeNode("图形")){
                //分别率设置
                if(ImGui::Button("设置为分辨率1024x1024")){
                    SDL_SetWindowSize(window,1024,1024);
                }

                //MSAA设置
                int antiAliasing=0;
                if(context->config.antiAliasing==render::AntiAliasing::MSAAx4){
                    antiAliasing=1;
                }
                if(ImGui::Combo("抗锯齿",&antiAliasing,"无\0SMAAx4")){
                    if(antiAliasing==1){
                        context->config.antiAliasing=render::AntiAliasing::MSAAx4;
                    }else{
                        context->config.antiAliasing=render::AntiAliasing::None;
                    }

                }

                ImGui::TreePop();
            }


            ImGui::End();
        } else {
            //处理视角移动

            if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                render::CameraController *camera = render->getCamera();
                camera->rotateAroundCenter(io.MouseDelta.x / 200, io.MouseDelta.y / 200);
            }


            if (io.MouseWheel) {
                render::CameraController *camera = render->getCamera();
                camera->moveForward(io.MouseWheel / 100);
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

    render->stop();
    saveProfile();

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
        delete[] (char*)frameBuffer;
       frameBuffer=new char[width*height*4];
    }
    memmove(frameBuffer,buffer,width*height*4);

    frameBuffer_h=height;
    frameBuffer_w=width;

    frameBufferLock.inUnlock();
}


void Gui::mousePosCallback(float x, float y) {
    if(mouseMoveFlag){
        float a=x-this->mouseMoveX;
        float b=y-this->mouseMoveY;
        this->mouseMoveX=x;
        this->mouseMoveY=y;

        printf("mouse\n");
        render::CameraController *camera=render->getCamera();
        camera->rotateAroundCenter(a/100.0f,b/100.0f);

    }else{
        this->mouseMoveX=x;
        this->mouseMoveY=y;
    }
}

//加载字体 加载配置文件
void Gui::loadProfile() {


    //加载配置文件
    std::ifstream profileFStream;
    profileFStream.open("StartRender.ini",std::ios::in);
    std::string line;
    while(profileFStream>>line){
        setProfile(line);
    }
    profileFStream.close();



    //加载天空盒
   // render->addSkyBoxImage("./assets/skybox/front.jpg","front");
   // render->addSkyBoxImage("./assets/skybox/back.jpg","back");
   // render->addSkyBoxImage("./assets/skybox/top.jpg","top");
   // render->addSkyBoxImage("./assets/skybox/bottom.jpg","bottom");
   // render->addSkyBoxImage("./assets/skybox/left.jpg","left");
   // render->addSkyBoxImage("./assets/skybox/right.jpg","right");
    render->addSkyBoxImage((const void*)R_FRONT_JPG_DATA,R_FRONT_JPG_SIZE-1,"front");
    render->addSkyBoxImage((const void*)R_BACK_JPG_DATA,R_BACK_JPG_SIZE-1,"back");
    render->addSkyBoxImage((const void*)R_LEFT_JPG_DATA,R_LEFT_JPG_SIZE-1,"left");
    render->addSkyBoxImage((const void*)R_RIGHT_JPG_DATA,R_RIGHT_JPG_SIZE-1,"right");
    render->addSkyBoxImage((const void*)R_TOP_JPG_DATA,R_TOP_JPG_SIZE-1,"top");
    render->addSkyBoxImage((const void*)R_BOTTOM_JPG_DATA,R_BOTTOM_JPG_SIZE-1,"bottom");


    loadFile(profile.file);

}

void Gui::saveProfile() {
    std::ofstream profileFStream;
    profileFStream.open("StartRender.ini",std::ios::out);
    if(!profile.file.empty()){
        profileFStream<<"File="<<profile.file<<std::endl;
    }
    profileFStream.close();
}

static void _split(const std::string &s, char delim,
                   std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    _split(s, delim, elems);
    return elems;
}

void Gui::setProfile(std::string line) {
    auto kv=split(line,'=');
    if(kv.size()>=2){
        if(kv[0]=="File"){
            this->profile.file=kv[1];
        }
    }
}

void Gui::loadFile(std::string file) {
//加载模型
    auto scene=render::Scene();
    if(!this->profile.file.empty()){
        auto *gltfFile=new GltfFile(file);
        gltfFile->fillScene(&scene);
        delete gltfFile;
    }
    render->setScene(scene);
}
