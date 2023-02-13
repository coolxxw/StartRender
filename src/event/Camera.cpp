//
// Created by xxw on 2022/11/11.
//
#include <cmath>
#include "Camera.h"
#include "../math/Matrix.h"

//绕轴旋转
Matrix4f rotateAroundAxis(Vector3f axis,float angle){
    float x=axis.x;
    float y=axis.y;
    float z=axis.z;
    float l= sqrt(x*x+y*y);
    float l2= sqrt(x*x+z*z);

    Matrix4f m={
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1
    };
    Matrix4f m1=m;
    Matrix4f m2=m;
    if(l!=0){
        //绕z轴方向旋转
        m1=Matrix4f{
                x/l,-y/l,0,0,
                y/l,x/l,0,0,
                0,0,1,0,
                0,0,0,1
        };
        m2=Matrix4f{
                x/l,y/l,0,0,
                -y/l,x/l,0,0,
                0,0,1,0,
                0,0,0,1
        };
    }

    Matrix4f m3=m;
    Matrix4f m4=m;
    if(l2!=0){
        //绕y轴旋转
        m3=Matrix4f{
                x/l2,0,z/l2,0,
                0,1,0,0,
                -z/l2,0,x/l2,0,
                0,0,0,1
        };
        m4=Matrix4f{
                x/l2,0,-z/l2,0,
                0,1,0,0,
                z/l2,0,x/l2,0,
                0,0,0,1
        };
    }



    Matrix4f m5=Matrix4f{
        1,0,0,0,
        0,cos(angle), sin(angle),0,
        0,-sin(angle), cos(angle),0,
        0,0,0,1
    };

    Matrix4f t= m2*m4*m5*m3*m1;
    return t;
}


Camera::Camera():e(1,0,0),t(0,0,1),g(-1,0,0) {

}

Camera Camera::moveForward(float m) {
    e=e+(g*m);
    return *this;
}

Camera Camera::rotateLeft(float r) {
    Vector3f l=t.cross(g);
    Vector3f g2=g*cos(r)+l*sin(r);
    g=g2;
    return *this;
}

Camera Camera::rotateUp(float r) {
    Vector3f t2=t* cos(r)-g*sin(r);
    Vector3f g2=g*cos(r)+t*sin(r);
    t=t2;
    g=g2;
    return *this;
}

Camera Camera::moveLeft(float m) {
    Vector3f l=t.cross(g);
    e=e+(l*m);
    return *this;
}

Camera Camera::rotateAroundCenter(float a, float b) {
    float L= sqrt(e.x*e.x+e.y*e.y+e.z*e.z);

    if(a!=0){
        //将e绕t旋转
        Vector4f vec=Vector4f(e.x,e.y,e.z,1);
        vec= rotateAroundAxis(t,a)*vec;
        e.x=vec.x/vec.w;
        e.y=vec.y/vec.w;
        e.z=vec.z/vec.w;
        float l= sqrt(e.x*e.x+e.y*e.y+e.z*e.z);

        g=-e;
        g=g*(1/l);
    }



    if(b!=0){
        Vector3f gt=g.cross(t);
        Vector4f vec=Vector4f(e.x,e.y,e.z,1);
        vec= rotateAroundAxis(gt,b)*vec;
        e.x=vec.x/vec.w;
        e.y=vec.y/vec.w;
        e.z=vec.z/vec.w;

        float l= sqrt(e.x*e.x+e.y*e.y+e.z*e.z);
        g=-e;
        g=g*(1/l);

        t=gt.cross(g);

    }

    float l= sqrt(e.x*e.x+e.y*e.y+e.z*e.z);
    e=e*(L/l);

    return *this;
}


