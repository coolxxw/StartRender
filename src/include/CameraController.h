//
// Created by xxw on 2023/2/21.
//

#ifndef STARTRENDER_CAMERACONTROLLER_H
#define STARTRENDER_CAMERACONTROLLER_H

#include "Camera.h"


namespace render{

    class CameraController :Camera{

    public:
        void  moveForward(float m);
        void  moveLeft(float m);
        void  rotateLeft(float r);
        void  rotateUp(float r);

        //将相机绕世界中心点选择,a为向屏幕右方向旋转弧度,b为向屏幕下方旋转弧度
        void  rotateAroundCenter(float a, float b);

    };
}




#endif //STARTRENDER_CAMERANODE_H
