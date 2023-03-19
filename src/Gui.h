//
// Created by xxw on 2023/2/20.
//

#ifndef STARTRENDER_GUI_H
#define STARTRENDER_GUI_H

#include "include/Render.h"


class InOutLock{
private:
    volatile bool in= false;
    volatile bool out= false;

public:

    void inLock(){
        while(true){
            while(out);
            in=true;
            if(out){
                in=false;
            }else{
                break;
            }
        }
    }

    void inUnlock(){
        in= false;
    }

    void outLock(){
        while(true){
            while(in);
            out=true;
            if(in){
                out=false;
            }else{
                break;
            }
        }
    }
    void outUnlock(){
        out=false;
    }



};

class GuiData;

class Gui : render::RenderPaintInterface{
private:
    GuiData* d;
    render::StartRender *render;
    void* frameBuffer= nullptr;
    InOutLock frameBufferLock;
    int frameBuffer_h=0;
    int frameBuffer_w=0;
    bool showController=false;
    bool mouseMoveFlag=false;
    float mouseMoveX=0;
    float mouseMoveY=0;



public:
    Gui();

    void exec();

    ~Gui();

    void keyCallback(int key, int scancode, int action, int mods);
    void mouseCallback(int button,int action,int mods);
    void mousePosCallback(float x,float y);

private:
    void paint(void *frameBuffer,int width,int height) override;

};


#endif //STARTRENDER_GUI_H
