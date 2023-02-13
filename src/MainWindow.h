//
// Created by xxw on 2022/11/10.
// 主窗口类

#ifndef STARTRENDER_MAINWINDOW_H
#define STARTRENDER_MAINWINDOW_H

#include "engine/Render.h"


class MainWindowPrivate;

class MainWindow :public RenderPaintInterface{
protected:
    MainWindowPrivate *d{};
    Render* render;
    friend MainWindowPrivate;
    bool mouseMoveWithLButtonFlag=false;
    int mousePosX;
    int mousePosY;

public:
    MainWindow();
    ~MainWindow();

    void show();
    void exec();

    void paint(void* frameBuffer)override;

protected:
    void keyboardHandle(int key);
    void mouseMoveWithLButtonHandle(int x,int y);
    void mouseWheelHandle(int x);

private:
    MainWindow(const MainWindow& w){}
    MainWindow operator=(const MainWindow& w){}


};


#endif //STARTRENDER_MAINWINDOW_H
