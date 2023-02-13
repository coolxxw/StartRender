//
// Created by xxw on 2022/11/11.
//

#ifndef STARTRENDER_CAMERA_H
#define STARTRENDER_CAMERA_H

#include "../math/Vector.h"

//相机
class Camera {
public:
    Vector3f e;
    Vector3f t;
    Vector3f g;


    Camera();

    Camera moveForward(float m);
    Camera moveLeft(float m);
    Camera rotateLeft(float r);
    Camera rotateUp(float r);

    //将相机绕世界中心点选择,a为向屏幕右方向旋转弧度,b为向屏幕下方旋转弧度
    Camera rotateAroundCenter(float a,float b);
};


#endif //STARTRENDER_CAMERA_H
