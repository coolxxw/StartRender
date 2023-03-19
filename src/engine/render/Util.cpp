//
// Created by xxw on 2023/2/22.
//

#include <cmath>
#include "../../include/Util.h"

using namespace render::Util;

void render::Util::VectorToAngle(Vector3f vector, float *angle_h, float *angle_v){
    static float pi= asin(1.0f)*2;
    auto x=(float)vector.x;
    auto y=(float )vector.y;
    auto z=(float)vector.z;

    float l= sqrt(x*x+y*y);
    if(l==0){
        *angle_h=0;
        if(z>0){
            *angle_v=90.0f;
        }else{
            *angle_v=-90.0f;
        }
        return;
    }

    float a;
    a= asin(abs(y/l));
    a=a/pi*180.0f;
    if(x>0){
        if(y>0){
            *angle_h=a;
        }else{
            *angle_h=-a+360.0f;
        }
    }else{
        if(y>0){
            *angle_h=-a+180.0f;
        }else{
            *angle_h=a+180.0f;
        }
    }

    l= sqrt(z*z+l*l);

    a= asin(abs(z/l));
    a=a/pi*180.0f;
    if(vector.z>0){
        *angle_v=a;
    }else {

        *angle_v = -a;
    }
}



Vector3f render::Util::AngleToVector(float angle_h, float angle_v){
    float x=1,y=1,z=1;
    static float pi= asin(1.0f)*2;

    float a=angle_h/180.0f*pi;

    x= cos(a);
    y= sin(a);

    a=angle_v/180.0f*pi;

    if(cos(a)==0){
        x=0;
        y=0;
        z= sin(a);
    }else{
        float l= sqrt(x*x+y*y);
        z=l* tan(a);
    }



    Vector3f vector(x,y,z);
    vector.normalization();
    return vector;

}
