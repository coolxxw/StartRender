//
// Created by xxw on 2022/11/11.
//
#include <cmath>
#include "Viewing.h"

Viewing::Viewing(const Camera &camera, ClipSpace clipSpace,int width,int height) {
    Vector3f w=-(camera.g);
    Vector3f v=camera.t;
    Vector3f u=v.cross(w);

    Matrix4f camMove=Matrix4f{1,0,0,-camera.e.x,
                              0,1,0,-camera.e.y,
                              0,0,1,-camera.e.z,
                              0,0,0,1};
    Matrix4f camRotate=Matrix4f{u.x,u.y,u.z,0,
                                v.x,v.y,v.z,0,
                                w.x,w.y,w.z,0,
                                0,0,0,1};
    cam=camRotate*camMove;//视口变换

    //透视投影
    //设t=1
    float t=clipSpace.t;
    float b=clipSpace.b;
    float n=clipSpace.n;
    float f=clipSpace.f;
    float r=clipSpace.r;
    float l=clipSpace.l;

    Matrix4f per=Matrix4f{2*n/(r-l),0,(l+r)/(l-r),0,
                          0,2*n/(t-b),(b+t)/(b-t),0,
                          0,0,(f+n)/(n-f),2*f*n/(f-n),
                          0,0,1,0};

    Matrix4f vp=Matrix4f{(float)height/2,0,0,(float )width/2,
                         0,(float )-height/2,0,(float )height/2,
                         0,0,1,0,
                         0,0,0,1};

    matrix=vp*per*cam;

}

Vector3f Viewing::projection(Vector3f vec) {
    Vector4f v=Vector4f(vec.x,vec.y,vec.z,1);
    v=matrix*v;
    return  Vector3f(v.x/v.w,v.y/v.w,v.z/v.w);
}

Vector3f Viewing::cameraTranform(Vector3f vec) {
    Vector4f v=Vector4f(vec.x,vec.y,vec.z,1);
    v=cam*v;
    return  Vector3f(v.x/v.w,v.y/v.w,v.z/v.w);
}


