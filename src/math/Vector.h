//
// Created by xxw on 2022/10/9.
//

#ifndef STARTRENDER_VECTOR_H
#define STARTRENDER_VECTOR_H

#include <cmath>

class Vector2f{
public:
    float x;
    float y;

    Vector2f():x(0),y(0){}

    Vector2f(float x,float y):x(x),y(y){}

    void set(float x,float y){
        this->x=x; this->y=y;
    }

    Vector2f operator+(const Vector2f& v){
        Vector2f vector;
        vector.x=this->x+v.x;
        vector.y=this->y+v.y;
        return vector;
    }

    Vector2f operator-(const Vector2f& v)const{
        Vector2f vector;
        vector.x=this->x-v.x;
        vector.y=this->y-v.y;
        return vector;
    }

    Vector2f operator-(){
        x=-x;
        y=-y;
        return *this;
    }

    /*点乘*/
    float operator*(const Vector2f& v){
        return x*v.x+y*v.y;
    }

    /*差积*/
    float cross(const Vector2f& v){
        return x*v.y-v.x*y;
    }

};


class Vector3f {
public:
    float x;
    float y;
    float z;

public:

    Vector3f(const Vector2f v,float z):x(v.x),y(v.y),z(z){}
    Vector3f(float x=0,float y=0,float z=0):x(x),y(y),z(z){}

    void set(float x,float y,float z){
        this->x=x; this->y=y; this->z=z;
    }

    operator Vector2f(){
        return Vector2f(x,y);
    }

    Vector3f operator+(const Vector3f& v){
        Vector3f vector;
        vector.x=this->x+v.x;
        vector.y=this->y+v.y;
        vector.z=this->z+v.z;
        return vector;
    }

    Vector3f operator-(const Vector3f& v)const{
        Vector3f vector;
        vector.x=this->x-v.x;
        vector.y=this->y-v.y;
        vector.z=this->z-v.z;
        return vector;
    }

    Vector3f operator-()const{
        return Vector3f(-x,-y,-z);
    }

    /*点乘*/
    Vector3f operator*(float k){
        return Vector3f(k*x,k*y,k*z);
    }
    float operator*(const Vector3f& v){
        return x*v.x+y*v.y+z*v.z;
    }

    /*差积*/
    Vector3f cross(const Vector3f& v){
        Vector3f vec;
        vec.x=y*v.z-v.y*z;
        vec.y=z*v.x-x*v.z;
        vec.z=x*v.y-y*v.x;
        return vec;
    }

    void normalization(){
        float l=sqrt(x*x+y*y+z*z);
        x/=l;
        y/=l;
        z/=l;
    }

};


class Vector4f {
public:
    float x;
    float y;
    float z;
    float w;

public:
    Vector4f(float x=0,float y=0,float z=0,float w=1):x(x),y(y),z(z),w(w){}

    void set(float x,float y,float z,float w=1){
        this->x=x; this->y=y; this->z=z; this->w=w;
    }

    Vector4f operator+(const Vector4f& v){
        Vector4f vector;
        vector.x=this->x+v.x;
        vector.y=this->x+v.y;
        vector.z=this->x+v.z;
        vector.w=this->x+v.w;
        return vector;
    }

    Vector4f operator-(const Vector4f& v){
        Vector4f vector;
        vector.x=this->x-v.x;
        vector.y=this->x-v.y;
        vector.z=this->x-v.z;
        vector.w=this->x-v.w;
        return vector;
    }

    /*点乘*/
    float operator*(const Vector4f& v){
        return x*v.x+y*v.y+z*v.z+w*v.w;
    }



    Vector3f vec3(){
        return Vector3f{x,y,z};
    }

};




#endif //SIMPLE3D_VECTOR_H
