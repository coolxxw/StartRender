//
// Created by xxw on 2022/10/9.
//

#ifndef STARTRENDER_MATRIX_H
#define STARTRENDER_MATRIX_H

#include "Vector.h"


class Matrix4f {
public:
    double m[4][4];


public:


    Matrix4f operator+(Matrix4f& matrix){
        Matrix4f m;
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                m.m[i][j]=this->m[i][j]+matrix.m[i][j];
            }
        }
        return m;
    }

    Matrix4f operator-(Matrix4f& matrix){
        Matrix4f m;
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                m.m[i][j]=this->m[i][j]-matrix.m[i][j];
            }
        }
        return m;
    }

    Matrix4f operator*(Matrix4f& matrix){
        Matrix4f m;
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                m.m[i][j]=0;
                for(int k=0;k<4;k++){
                    m.m[i][j]+=this->m[i][k]*matrix.m[k][j];
                }

            }
        }
        return m;
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


    Matrix4f inverse();//求逆

};


#endif //STARTRENDER_MATRIX_H
