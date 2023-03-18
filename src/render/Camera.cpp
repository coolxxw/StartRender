//
// Created by xxw on 2023/2/21.
//

#include "../include/Camera.h"
#include "../engine/data/Context.h"
#include "../math/Matrix.h"

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




void  Camera::moveForward(double m) {
    Vector3f &e=context->camera.e;
    Vector3f &g=context->camera.g;
    e=e+(g*(float )m);

}

void  Camera::rotateLeft(double r) {
    Vector3f &e=context->camera.e;
    Vector3f &g=context->camera.g;
    Vector3f &t=context->camera.t;
    Vector3f l=t.cross(g);
    Vector3f g2=g*cos(r)+l*sin(r);
    g=g2;

}

void  Camera::rotateUp(double r) {
    Vector3f &e=context->camera.e;
    Vector3f &g=context->camera.g;
    Vector3f &t=context->camera.t;

    Vector3f t2=t* cos(r)-g*sin(r);
    Vector3f g2=g*cos(r)+t*sin(r);
    t=t2;
    g=g2;

}

void  Camera::moveLeft(double m) {
    Vector3f &e=context->camera.e;
    Vector3f &g=context->camera.g;
    Vector3f &t=context->camera.t;

    Vector3f l=t.cross(g);
    e=e+(l*m);

}

void  Camera::rotateAroundCenter(double a, double b) {
    if(a==0 && b==0){
        return;
    }
    Vector3f &e=context->camera.e;
    Vector3f &g=context->camera.g;
    Vector3f &t=context->camera.t;

    Vector3f left=g.cross(t);
    //旋转反向
    Vector3f r=left*a-t*b;
    Vector3f axis=r.cross(g);
    Matrix3f m= rotateAroundAxis(axis, sqrt(a*a+b*b));
    e=m*e;
    t=m*t;
    g=-e;
    g.normalization();


}


