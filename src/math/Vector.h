//
// Created by xxw on 2022/10/9.
//

#ifndef STARTRENDER_VECTOR_H
#define STARTRENDER_VECTOR_H

#include <cmath>

#include "../platform/def.h"
#include "../include/type.h"

class Vector2f{
public:
    decimal x;
    decimal y;

    Vector2f():x(0),y(0){}

    Vector2f(decimal x,decimal y):x(x),y(y){}


    Vector2f operator+(const Vector2f& v) const{
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
    decimal operator*(const Vector2f& v) const{
        return x*v.x+y*v.y;
    }

    /*差积*/
    decimal cross(const Vector2f& v) const{
        return x*v.y-v.x*y;
    }

};


class Vector3f {
public:
    decimal x;
    decimal y;
    decimal z;

public:

    Vector3f(const Vector2f v,decimal z):x(v.x),y(v.y),z(z){}
    explicit Vector3f(decimal x=0,decimal y=0,decimal z=0):x(x),y(y),z(z){}

    operator Vector2f() const{
        return {x,y};
    }

    Vector3f operator+(const Vector3f& v) const{
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
    Vector3f operator*(decimal k) const{
        return Vector3f(k*x,k*y,k*z);
    }
    decimal operator*(const Vector3f& v) const{
        return x*v.x+y*v.y+z*v.z;
    }

    /*差积*/
    Vector3f cross(const Vector3f& v) const{
        Vector3f vec;
        vec.x=y*v.z-v.y*z;
        vec.y=z*v.x-x*v.z;
        vec.z=x*v.y-y*v.x;
        return vec;
    }

    void normalization(){
        decimal l=sqrt(x*x+y*y+z*z);
        x/=l;
        y/=l;
        z/=l;
    }

};


class Vector4f {
public:
    decimal x;
    decimal y;
    decimal z;
    decimal w;

public:
    explicit Vector4f(decimal x=0,decimal y=0,decimal z=0,decimal w=1):x(x),y(y),z(z),w(w){}

    Vector4f operator+(const Vector4f& v) const{
        Vector4f vector;
        vector.x=this->x+v.x;
        vector.y=this->x+v.y;
        vector.z=this->x+v.z;
        vector.w=this->x+v.w;
        return vector;
    }

    Vector4f operator-(const Vector4f& v) const{
        Vector4f vector;
        vector.x=this->x-v.x;
        vector.y=this->x-v.y;
        vector.z=this->x-v.z;
        vector.w=this->x-v.w;
        return vector;
    }

    /*点乘*/
    decimal operator*(const Vector4f& v) const{
        return x*v.x+y*v.y+z*v.z+w*v.w;
    }

    /*点乘*/
    Vector4f operator*(double k) const{
        return Vector4f(x*k,y*k,z*k,w*k);
    }



    Vector3f vec3() const{
        return Vector3f{x,y,z};
    }

};



#endif //STARTRENDER_VECTOR_H
