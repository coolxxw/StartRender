//
// Created by xxw on 2023/2/20.
//

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include "imgui-knobs.h"
#include <stdio.h>
#include <fstream>
#include "include/Util.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>

#include "Gui.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Gui* gui= (Gui*)glfwGetWindowUserPointer(window);
    if(gui){
        gui->keyCallback(key,scancode,action,mods);
    }

}

static void  mouse_callback(GLFWwindow* window,int button,int action,int mods){
    Gui* gui= (Gui*)glfwGetWindowUserPointer(window);
    if(gui){
        gui->mouseCallback(button,action,mods);
    }
}

static void mousePos_callback(GLFWwindow* window,double x,double y){
    Gui* gui= (Gui*)glfwGetWindowUserPointer(window);
    if(gui){
        gui->mousePosCallback(x,y);
    }
}




Gui::Gui() {
    render=new Render::StartRender(0,0);
}

void Gui::exec() {

        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
            return ;

        // Create window with graphics context
        GLFWwindow* window = glfwCreateWindow(1024, 1024, "StartRender", NULL, NULL);
        if (window == NULL)
            return ;
        glfwSetWindowUserPointer(window,this);
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL2_Init();

        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You can also loadGlb multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
        // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
        // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
        // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
        // - Read 'docs/FONTS.md' for more instructions and details.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        //io.Fonts->AddFontDefault();
        //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
        //IM_ASSERT(font != NULL);

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


        // Our state
        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        //键盘事件
        glfwSetKeyCallback(window,key_callback);
        //鼠标事件
        //glfwSetMouseButtonCallback(window,mouse_callback);
        //glfwSetCursorPosCallback(window,mousePos_callback);

        render->registerPaintImpl(this);
        render->start();

        // Main loop
        while (!glfwWindowShouldClose(window))
        {


            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            glfwPollEvents();





            // Start the Dear ImGui frame
            ImGui_ImplOpenGL2_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            int display_w=0, display_h=0;
            glfwGetFramebufferSize(window, &display_w, &display_h);

            if(showController){
                ImGui::Begin("Controller");
                ImGui::Text("3D渲染引擎");
                ImGui::Text("渲染分辨率: %dx%d",display_w,display_h);
                ImGui::Text("渲染帧率: %.1f FPS  %lld",render->getFps(),render->getFrameCounter());
                ImGui::Text("输出帧率: %.1f FPS",  ImGui::GetIO().Framerate);

                if(ImGui::TreeNode("光照")){
                    Render::LightParam &lightParam=render->getLightParam();
                    ImGui::SeparatorText("默认直射光");
                    static float knobh=0,knobv=0;
                    Render::Util::VectorToAngle(lightParam.directionalLight,&knobh,&knobv);
                    ImGui::SliderFloat("水平方向",&knobh,0,360);
                    ImGui::SliderFloat("垂直方向",&knobv,-90,90);
                    lightParam.directionalLight=Render::Util::AngleToVector(knobh,knobv);

                    ImGui::SeparatorText("漫反射");
                    ImGui::SliderFloat("漫反射系数",&lightParam.diffuse.a,0,1.0f);
                    ImGui::SeparatorText("镜面反射");
                    ImGui::SliderFloat("镜反射系数",&lightParam.specular.a,0,1.0f);
                    ImGui::SliderInt("指数",&lightParam.specularExponent,1,256);
                    ImGui::SeparatorText("环境光");
                    ImGui::SliderFloat("环境光系数",&lightParam.ambient.a,0,1.0f);
                    ImGui::TreePop();
                }



                ImGui::End();
            }else{

                if( ImGui::IsMouseDown(ImGuiMouseButton_Left)){
                        Render::Camera camera=render->getCamera();
                        camera.rotateAroundCenter(io.MouseDelta.x/200,io.MouseDelta.y/200);
                }


                if(io.MouseWheel){
                    Render::Camera camera=render->getCamera();
                    camera.moveForward(io.MouseWheel/100);
                }

            }



            // Rendering
            ImGui::Render();

            {


                render->setSize(display_w,display_h);

                glViewport(0, 0, display_w, display_h);
                if(display_h==frameBuffer_h && display_w==frameBuffer_w){
                    frameBufferLock.outLock();
                    glDrawPixels(display_w,display_h,GL_RGBA,GL_UNSIGNED_BYTE,frameBuffer);
                    frameBufferLock.outUnlock();
                }



            }
            //glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
            //glClear(GL_COLOR_BUFFER_BIT);

            // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
            // you may need to backup/reset/restore other state, e.g. for current shader using the commented lines below.
            //GLint last_program;
            //glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
            //glUseProgram(0);
            ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
            //glUseProgram(last_program);

            glfwMakeContextCurrent(window);
            glfwSwapBuffers(window);
        }

        // Cleanup
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();

        return ;


}

Gui::~Gui() {
    delete render;
    delete[] (char*)frameBuffer;
}

void Gui::paint(void *buffer, int width, int height) {
    if(width*height==0){
        return;
    }
    frameBufferLock.inLock();
    if(frameBuffer_w!=width || frameBuffer_h!=width){

        delete[] (char*)frameBuffer;
        frameBuffer=new char[width*height*4];

    }
    memmove(frameBuffer,buffer,width*height*4);
    frameBuffer_h=height;
    frameBuffer_w=width;

    frameBufferLock.inUnlock();
}

void Gui::keyCallback(int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_F1 && action == GLFW_RELEASE){
        showController=!showController;
    }



}

void Gui::mouseCallback(int button, int action, int mods) {
    if(button==GLFW_MOUSE_BUTTON_LEFT){
        if(action==GLFW_PRESS){
              if(mouseMoveFlag){

              }else{
                  mouseMoveFlag=true;

              }

        }else if(action==GLFW_RELEASE){
                mouseMoveFlag= false;
        }
    }
}

void Gui::mousePosCallback(double x, double y) {
    if(mouseMoveFlag){
        double a=x-mouseMoveX;
        double b=y-mouseMoveY;
        mouseMoveX=x;
        mouseMoveY=y;

        printf("mouse\n");
        Render::Camera camera=render->getCamera();
        camera.rotateAroundCenter(a/100,b/100);

    }else{
        mouseMoveX=x;
        mouseMoveY=y;
    }
}
