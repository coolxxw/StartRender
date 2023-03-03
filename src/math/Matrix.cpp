//
// Created by xxw on 2022/10/9.
//

#include <cmath>
#include "Matrix.h"

Matrix4f Matrix4f::inverse() {
    Matrix4f inv{};

    //求行列式
    double det= (m[1][1]*m[2][2]*m[3][3]+m[1][2]*m[2][3]*m[3][1]+m[1][3]*m[2][1]*m[3][2]-m[1][3]*m[2][2]*m[3][1]-m[1][1]*m[2][3]*m[3][2]-m[1][2]*m[2][1]*m[3][3])*m[0][0]
    -(m[0][1]*m[2][2]*m[3][3]+m[0][2]*m[2][3]*m[3][1]+m[0][3]*m[2][1]*m[3][2]-m[0][3]*m[2][2]*m[3][1]-m[0][1]*m[2][3]*m[3][2]-m[0][2]*m[2][1]*m[3][3])*m[1][0]
    +(m[0][1]*m[1][2]*m[3][3]+m[0][2]*m[1][3]*m[3][1]+m[0][3]*m[1][1]*m[3][2]-m[0][3]*m[1][2]*m[3][1]-m[0][1]*m[1][3]*m[3][2]-m[0][2]*m[1][1]*m[3][3])*m[2][0]
    -(m[0][1]*m[1][2]*m[2][3]+m[0][2]*m[1][3]*m[2][1]+m[0][3]*m[1][1]*m[2][2]-m[0][3]*m[1][2]*m[2][1]-m[0][1]*m[1][3]*m[2][2]-m[0][2]*m[1][1]*m[2][3])*m[3][0];


    if(det==0)
    {
        for(auto & i : inv.m){
            for(float & j : i){
                j=NAN;
            }
        }
        return inv;
    }



    inv.m[0][0]=+(m[1][1]*m[2][2]*m[3][3]+m[1][2]*m[2][3]*m[3][1]+m[1][3]*m[2][1]*m[3][2]-m[1][3]*m[2][2]*m[3][1]-m[1][1]*m[2][3]*m[3][2]-m[1][2]*m[2][1]*m[3][3])/det;
    inv.m[0][1]=-(m[0][1]*m[2][2]*m[3][3]+m[0][2]*m[2][3]*m[3][1]+m[0][3]*m[2][1]*m[3][2]-m[0][3]*m[2][2]*m[3][1]-m[0][1]*m[2][3]*m[3][2]-m[0][2]*m[2][1]*m[3][3])/det;
    inv.m[0][2]=+(m[0][1]*m[1][2]*m[3][3]+m[0][2]*m[1][3]*m[3][1]+m[0][3]*m[1][1]*m[3][2]-m[0][3]*m[1][2]*m[3][1]-m[0][1]*m[1][3]*m[3][2]-m[0][2]*m[1][1]*m[3][3])/det;
    inv.m[0][3]=-(m[0][1]*m[1][2]*m[2][3]+m[0][2]*m[1][3]*m[2][1]+m[0][3]*m[1][1]*m[2][2]-m[0][3]*m[1][2]*m[2][1]-m[0][1]*m[1][3]*m[2][2]-m[0][2]*m[1][1]*m[2][3])/det;

    inv.m[1][0]=-(m[1][0]*m[2][2]*m[3][3]+m[1][2]*m[2][3]*m[3][0]+m[1][3]*m[2][0]*m[3][2]-m[1][3]*m[2][2]*m[3][0]-m[1][0]*m[2][3]*m[3][2]-m[1][2]*m[2][0]*m[3][3])/det;
    inv.m[1][1]=+(m[0][0]*m[2][2]*m[3][3]+m[0][2]*m[2][3]*m[3][0]+m[0][3]*m[2][0]*m[3][2]-m[0][3]*m[2][2]*m[3][0]-m[0][0]*m[2][3]*m[3][2]-m[0][2]*m[2][0]*m[3][3])/det;
    inv.m[1][2]=-(m[0][0]*m[1][2]*m[3][3]+m[0][2]*m[1][3]*m[3][0]+m[0][3]*m[1][0]*m[3][2]-m[0][3]*m[1][2]*m[3][0]-m[0][0]*m[1][3]*m[3][2]-m[0][2]*m[1][0]*m[3][3])/det;
    inv.m[1][3]=+(m[0][0]*m[1][2]*m[2][3]+m[0][2]*m[1][3]*m[2][0]+m[0][3]*m[1][0]*m[2][2]-m[0][3]*m[1][2]*m[2][0]-m[0][0]*m[1][3]*m[2][2]-m[0][2]*m[1][0]*m[2][3])/det;

    inv.m[2][0]=+(m[1][0]*m[2][1]*m[3][3]+m[1][1]*m[2][3]*m[3][0]+m[1][3]*m[2][0]*m[3][1]-m[1][3]*m[2][1]*m[3][0]-m[1][0]*m[2][3]*m[3][1]-m[1][1]*m[2][0]*m[3][3])/det;
    inv.m[2][1]=-(m[0][0]*m[2][1]*m[3][3]+m[0][1]*m[2][3]*m[3][0]+m[0][3]*m[2][0]*m[3][1]-m[0][3]*m[2][1]*m[3][0]-m[0][0]*m[2][3]*m[3][1]-m[0][1]*m[2][0]*m[3][3])/det;
    inv.m[2][2]=+(m[0][0]*m[1][1]*m[3][3]+m[0][1]*m[1][3]*m[3][0]+m[0][3]*m[1][0]*m[3][1]-m[0][3]*m[1][1]*m[3][0]-m[0][0]*m[1][3]*m[3][1]-m[0][1]*m[1][0]*m[3][3])/det;
    inv.m[2][3]=-(m[0][0]*m[1][1]*m[2][3]+m[0][1]*m[1][3]*m[2][0]+m[0][3]*m[1][0]*m[2][1]-m[0][3]*m[1][1]*m[2][0]-m[0][0]*m[1][3]*m[2][1]-m[0][1]*m[1][0]*m[2][3])/det;

    inv.m[3][0]=-(m[1][0]*m[2][1]*m[3][2]+m[1][1]*m[2][2]*m[3][0]+m[1][2]*m[2][0]*m[3][1]-m[1][2]*m[2][1]*m[3][0]-m[1][0]*m[2][2]*m[3][1]-m[1][1]*m[2][0]*m[3][2])/det;
    inv.m[3][1]=+(m[0][0]*m[2][1]*m[3][2]+m[0][1]*m[2][2]*m[3][0]+m[0][2]*m[2][0]*m[3][1]-m[0][2]*m[2][1]*m[3][0]-m[0][0]*m[2][2]*m[3][1]-m[0][1]*m[2][0]*m[3][2])/det;
    inv.m[3][2]=-(m[0][0]*m[1][1]*m[3][2]+m[0][1]*m[1][2]*m[3][0]+m[0][2]*m[1][0]*m[3][1]-m[0][2]*m[1][1]*m[3][0]-m[0][0]*m[1][2]*m[3][1]-m[0][1]*m[1][0]*m[3][2])/det;
    inv.m[3][3]=+(m[0][0]*m[1][1]*m[2][2]+m[0][1]*m[1][2]*m[2][0]+m[0][2]*m[1][0]*m[2][1]-m[0][2]*m[1][1]*m[2][0]-m[0][0]*m[1][2]*m[2][1]-m[0][1]*m[1][0]*m[2][2])/det;


    return inv;

}
