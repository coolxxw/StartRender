//内存泄漏检测
#ifdef _DEBUG
#include <vld.h>
#endif

#include <iostream>
#include "include/Render.h"
#include "include/GltfFile.h"
#include "Gui.h"


int main(){
    Gui gui;
    gui.exec();

    return 0;

}
