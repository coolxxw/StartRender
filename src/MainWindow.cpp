//
// Created by xxw on 2022/11/10.
//

#include <windows.h>
#include "MainWindow.h"
#include "engine/Render.h"


class MainWindowPrivate{
public:
    MainWindow* outer;
    HWND hWnd;
    HBITMAP hBitmap;
    void *pBits;
    int width;
    int height;

    LRESULT  wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void createDIB(HDC hdc);
    void updateBitmap(void* bits);
};






void MainWindowPrivate::updateBitmap(void *bits) {
    if(bits== nullptr){
        return;
    }
    static unsigned char j = 0;
    if (pBits == NULL) return;


    for (int i = 0; i < height; i++) {
        memcpy((byte*)pBits+ ((size_t)i)*width*4, (byte *)bits+ (((size_t)height-1-i)*width*4), (size_t)width*4);
    }
    // for (int i = 0; i < height*width*4; i++) {
    //     *((unsigned char*)pBits+i) = j;
    //      j += rand();
    // }
    //UpdateWindow(hwnd);
    //ShowWindow(hwnd, SW_NORMAL);



   // if (isPainted)

   // {
        // RedrawWindow(hwnd,NULL,NULL, RDW_INTERNALPAINT);
        //isPainted = false;
        InvalidateRect(hWnd, NULL, false);
    //    isPainted = false;
        UpdateWindow(hWnd);
    //}

}


static LRESULT  WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    MainWindowPrivate* obj= (MainWindowPrivate*)::GetWindowLongPtr(hwnd,GWLP_USERDATA);
    if(obj){
        return obj->wndProc(hwnd,message,wParam,lParam);
    }else{
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

LRESULT  MainWindowPrivate::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    HDC         hdc;  //设备环境句柄
    HDC hdcMem = NULL;;
    PAINTSTRUCT ps;
    RECT        rect;
    BITMAP bitmap = {0};

    switch (message) {

        case WM_CREATE:

            return 0;
            //窗口绘制消息
        case WM_PAINT:


            hdc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &rect);

            GetObject(hBitmap, sizeof(BITMAP), &bitmap);
            hdcMem = CreateCompatibleDC(hdc);
            SelectObject(hdcMem, hBitmap);

            StretchBlt(hdc, 0, 0, width, height , hdcMem,0,0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
            //DeleteObject(hBitmap);
            DeleteDC(hdcMem);


            EndPaint(hwnd, &ps);
            return 0;

        case WM_KEYDOWN:
            outer->keyboardHandle(wParam);
            return 0;

        case WM_LBUTTONDOWN:
            outer->mouseMoveWithLButtonFlag=true;
            outer->mousePosX= LOWORD(lParam);
            outer->mousePosY= HIWORD(lParam);
            return 0;

        case WM_LBUTTONUP:
            outer->mouseMoveWithLButtonFlag=false;
            return 0;

        case WM_MOUSELEAVE:
            outer->mouseMoveWithLButtonFlag=false;

        case WM_MOUSEMOVE:
            if(outer->mouseMoveWithLButtonFlag){
                outer->mouseMoveWithLButtonHandle(LOWORD(lParam), HIWORD(lParam));
                return 0;
            }
            break;

        case WM_MOUSEWHEEL:
        {
            int n=HIWORD(wParam);
            short s=*(short*)(&n);
            outer->mouseWheelHandle(s);
        }

            return 0;


            //窗口销毁消息
        case WM_DESTROY:
            pBits = NULL;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}


MainWindow::MainWindow() {

    this->d=new MainWindowPrivate;
    d->outer=this;
    d->width=600;
    d->height=600;

    static TCHAR szClassName[] = TEXT("StartRender");  //窗口类名


    static WNDCLASS wndclass = { 0 };
    wndclass.style = CS_HREDRAW | CS_VREDRAW;  //窗口风格
    wndclass.lpfnWndProc = WndProc;  //窗口过程
    wndclass.cbClsExtra = 0;  //暂时不需要理解
    wndclass.cbWndExtra = 0;  //暂时不需要理解
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);  //窗口图标
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);  //鼠标样式
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  //窗口背景画刷
    wndclass.lpszMenuName = NULL;  //窗口菜单
    wndclass.lpszClassName = szClassName;  //窗口类名
    //注册窗口
    RegisterClass(&wndclass);
    /*****第②步：创建窗口(并让窗口显示出来)*****/
    this->d->hWnd = CreateWindow(
            szClassName,  //窗口类的名字
            TEXT("StartRender"),  //窗口标题（出现在标题栏）
            WS_OVERLAPPEDWINDOW,  //窗口风格
            CW_USEDEFAULT,  //初始化时x轴的位置
            CW_USEDEFAULT,  //初始化时y轴的位置
            d->width,    //窗口宽度
            d->height,  //窗口高度
            NULL,  //父窗口句柄
            NULL,  //窗口菜单句柄
            NULL,  //当前窗口的句柄
            NULL  //不使用该值
    );
    //将对象指针关联到窗口句柄
    ::SetWindowLongPtr(d->hWnd, GWLP_USERDATA, (LONG_PTR)(this->d));
    d->createDIB(NULL);
    //显示窗口
    ShowWindow(d->hWnd, SW_NORMAL);
    //更新（绘制）窗口
    UpdateWindow(d->hWnd);


}

MainWindow::~MainWindow() {

}

void MainWindow::show() {

}

void MainWindow::exec() {
    render=new Render(d->width,d->height);
    render->registerPaintImpl(this);
    render->start();

    MSG      msg;  //消息

    /**********第③步：消息循环**********/
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);  //翻译消息
        DispatchMessage(&msg);  //分派消息
    }
}

void MainWindow::paint(void *frameBuffer) {
    d->updateBitmap(frameBuffer);
}

void MainWindow::keyboardHandle(int key) {
    switch (key) {

        case 37://反向键左
            render->setCamera(render->getCamera().rotateLeft(0.1));
            break;

        case 38://上
            render->setCamera(render->getCamera().rotateUp(0.1));
            break;

        case 39://右
            render->setCamera(render->getCamera().rotateLeft(-0.1));
            break;
        case 40://下
            render->setCamera(render->getCamera().rotateUp(-0.1));
            break;

        case 'W':
            render->setCamera(render->getCamera().moveForward(0.01));
            break;
        case 'S':
            render->setCamera(render->getCamera().moveForward(-0.01));
            break;
        case 'A':
            render->setCamera(render->getCamera().moveLeft(0.01));
            break;
        case 'D':
            render->setCamera(render->getCamera().moveLeft(-0.01));
            break;

    }
}

void MainWindow::mouseMoveWithLButtonHandle(int x,int y) {
    float a=x-mousePosX;
    float b=y-mousePosY;
    mousePosX=x;
    mousePosY=y;
    render->setCamera(render->getCamera().rotateAroundCenter(a/100,b/100));

}

void MainWindow::mouseWheelHandle(int x) {
    render->setCamera(render->getCamera().moveForward((double)x/10000));
}


void MainWindowPrivate::createDIB(HDC hdc) {
    BITMAPINFOHEADER header = { 0 };
    header.biSize = 40;
    header.biWidth = width;
    header.biHeight = height;
    header.biPlanes = 1;
    header.biBitCount = 32;
    header.biCompression = BI_RGB;

    tagBITMAPINFO   info = { 0 };
    info.bmiHeader = header;


    hBitmap = CreateDIBSection(hdc, &info, DIB_RGB_COLORS, &(pBits), NULL, NULL);
}



