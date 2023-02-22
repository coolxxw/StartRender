//内存泄漏检测
#ifdef _DEBUG
#include <vld.h>
#endif

#include <log4cpp/config-win32.h>

#include "old/MainWindow.h"
#include "object/GltfFile.h"
#include "Gui.h"

int main(){
    Gui gui;
    gui.exec();

    //MainWindow w;
    //w.exec();
    //Object3D obj;
    //GltfFile gltf=GltfFile("Bee.glb");
    //Object3D *obj= gltf.newMesh();

    return 0;
}
