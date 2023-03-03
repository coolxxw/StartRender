//
// Created by xxw on 2022/10/9.
//

#ifndef STARTRENDER_MATRIX_H
#define STARTRENDER_MATRIX_H

#include "Vector.h"


class Matrix3f{
public:
    float m[3][3]{
        1,0,0,
        0,1,0,
        0,0,1
    };

    Matrix3f operator+(Matrix3f& matrix){
        Matrix3f m2{};
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                m2.m[i][j]=this->m[i][j]+matrix.m[i][j];
            }
        }
        return m2;
    }

    Matrix3f operator-(Matrix3f& matrix){
        Matrix3f m2{};
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                m2.m[i][j]=this->m[i][j]-matrix.m[i][j];
            }
        }
        return m2;
    }

    Matrix3f operator*(Matrix3f& matrix){
        Matrix3f m2{};
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                m2.m[i][j]=0;
                for(int k=0;k<3;k++){
                    m2.m[i][j]+=this->m[i][k]*matrix.m[k][j];
                }

            }
        }
        return m2;
    }

    //矩阵乘向量
    Vector3f operator*(Vector3f& v){
        Vector3f vec;
        vec.x=v.x*m[0][0]+v.y*m[0][1]+v.z*m[0][2];
        vec.y=v.x*m[1][0]+v.y*m[1][1]+v.z*m[1][2];
        vec.z=v.x*m[2][0]+v.y*m[2][1]+v.z*m[2][2];
        return vec;
    }



};

class Matrix4f {
public:
    float m[4][4]{1,0,0,0,
                    0,1,0,0,
                    0,0,1,0,
                    0,0,0,1};


public:


    Matrix4f operator+(Matrix4f& matrix){
        Matrix4f m2{};
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                m2.m[i][j]=this->m[i][j]+matrix.m[i][j];
            }
        }
        return m2;
    }

    Matrix4f operator-(Matrix4f& matrix){
        Matrix4f m2{};
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                m2.m[i][j]=this->m[i][j]-matrix.m[i][j];
            }
        }
        return m2;
    }

    Matrix4f operator*(Matrix4f& matrix){
        Matrix4f m2{};
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                m2.m[i][j]=0;
                for(int k=0;k<4;k++){
                    m2.m[i][j]+=this->m[i][k]*matrix.m[k][j];
                }

            }
        }
        return m2;
    }

    //矩阵乘向量
    Vector4f operator*(Vector4f& v){
        Vector4f vec;
        vec.x=v.x*m[0][0]+v.y*m[0][1]+v.z*m[0][2]+v.w*m[0][3];
        vec.y=v.x*m[1][0]+v.y*m[1][1]+v.z*m[1][2]+v.w*m[1][3];
        vec.z=v.x*m[2][0]+v.y*m[2][1]+v.z*m[2][2]+v.w*m[2][3];
        vec.w=v.x*m[3][0]+v.y*m[3][1]+v.z*m[3][2]+v.w*m[3][3];

        return vec;
    }

    void move(Vector3f vec){
        Matrix4f t={
                1,0,0,vec.x,
                0,1,0,vec.y,
                0,0,1,vec.z,
                0,0,0,1
        };
        *this=t*(*this);
    }

    void scale(Vector3f vec){
        Matrix4f t={
                vec.x,0,0,0,
                0,vec.y,0,0,
                0,0,vec.z,0,
                0,0,0,1
        };
        *this=t*(*this);
    }


    Matrix4f inverse();//求逆

};


#endif //STARTRENDER_MATRIX_H
