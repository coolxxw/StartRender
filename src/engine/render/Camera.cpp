//
// Created by xxw on 2023/2/21.
//

#include "../../include/CameraController.h"
#include "../data/Context.h"


using namespace render;

//绕轴旋转
Matrix3f rotateAroundAxis(Vector3f axis,float angle){
   axis.normalization();
   float x=axis.x;
   float y=axis.y;
   float z=axis.z;

   float s= sin(angle);
   float c= cos(angle);

   return Matrix3f{
       x*x*(1-c)+c,x*y*(1-c)+z*s,x*z*(1-c)-y*s,
       x*y*(1-c)-z*s,y*y*(1-c)+c,y*z*(1-c)+x*s,
       x*z*(1-c)+y*s,y*z*(1-c)-x*s,z*z*(1-c)+c,
   };


}




void  CameraController::moveForward(float m) {
    e=e+(g*(float )m);
}

void  CameraController::rotateLeft(float r) {
    Vector3f l=t.cross(g);
    Vector3f g2=g*cosf(r)+l*sinf(r);
    g=g2;

}

void  CameraController::rotateUp(float r) {
    Vector3f t2=t* cos(r)-g*sin(r);
    Vector3f g2=g*cos(r)+t*sin(r);
    t=t2;
    g=g2;

}

void  CameraController::moveLeft(float m) {
    Vector3f l=t.cross(g);
    e=e+(l*m);

}

void  CameraController::rotateAroundCenter(float a, float b) {
    if(a==0 && b==0){
        return;
    }

    Vector3f left=g.cross(t);
    //旋转反向
    Vector3f r=left*a-t*b;
    Vector3f axis=r.cross(g);
    Matrix3f m= rotateAroundAxis(axis, sqrtf(a*a+b*b));
    e=m*e;
    t=m*t;
    g=-e;
    g.normalization();


}


