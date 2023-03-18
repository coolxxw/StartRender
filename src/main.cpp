//内存泄漏检测
#ifdef _DEBUG
#include <vld.h>
#endif

#include "Gui.h"


int main(){
    Gui gui;
    gui.exec();

    return 0;

}

