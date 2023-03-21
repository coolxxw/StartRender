//
// Created by xxw on 2023/3/8.
//

#include "RasterizationShader.h"
using render_core::RasterizationShader;


static void shading1(unsigned int width, unsigned int height, float *zbuffer,
                     const unsigned int *indices,
                     unsigned int indicesCount,
                     const Vector4f *vertex) <%
    {
        auto count = indicesCount / 3;
        for (auto index = 0; index < count; index++) {
            auto a = vertex[indices[index * 3]];
            auto b = vertex[indices[index * 3 + 1]];
            auto c = vertex[indices[index * 3 + 2]];
            auto left = (unsigned int) floor(fmax(0, fmin(a.x, fmin(b.x, c.x))));
            auto right = (unsigned int) ceil(fmin(width, fmax(0, fmax(a.x, fmax(b.x, c.x)))));
            auto top = (unsigned int) floor(fmax(0, fmin(a.y, fmin(b.y, c.y))));
            auto bottom = (unsigned int) ceil(fmin(height, fmax(0, fmax(a.y, fmax(b.y, c.y)))));

            float S1 = (-(a.x - b.x) * (c.y - b.y) + (a.y - b.y) * (c.x - b.x));
            float S2 = (-(b.x - c.x) * (a.y - c.y) + (b.y - c.y) * (a.x - c.x));


            for (int i = left; i < right; i++) {
                for (int j = top; j < bottom; j++) {
                    float x = (float) i + 0.5;
                    float y = (float) j + 0.5;

                    float ha = (-(x - b.x) * (c.y - b.y) + (y - b.y) * (c.x - b.x))
                               / S1;
                    float hb = (-(x - c.x) * (a.y - c.y) + (y - c.y) * (a.x - c.x))
                               / S2;
                    float hc = 1 - ha - hb;

                    //if ha>0 && hb>0 && hc>0
                    if (!((*(unsigned int *) &ha | *(unsigned int *) &hb | *(unsigned int *) &hc) & 0x80000000)) {
                        float z = a.z * ha + b.z * hb + c.z * hc + 1; // 映射到0-2 值越大离相机越近
                        if (z < 2 && z > zbuffer[j * width + i]) {
                            zbuffer[j * width + i] = z;
                        }

                    }
                }
            }
        }
    }

%>


static void shading2(
        unsigned int width, unsigned int height, float *zBuffer,
        const unsigned int *indices,
        unsigned int indicesCount,
        const Vector4f *vertex) {

    auto count=indicesCount/3;
    for (auto index=0;index<count;index++){
        auto a=vertex[indices[index*3]];
        auto b=vertex[indices[index*3+1]];
        auto c=vertex[indices[index*3+2]];

        unsigned int left,right,top,bottom;

        if(false){
            left= (unsigned int) floor(fmax(0,fmin(a.x,fmin(b.x,c.x))));
            right=(unsigned int) ceil(fmin(width,fmax(0,fmax(a.x,fmax(b.x,c.x)))));
            top =(unsigned int) floor(fmax(0,fmin(a.y,fmin(b.y,c.y))));
            bottom=(unsigned int) ceil(fmin(height,fmax(0,fmax(a.y,fmax(b.y,c.y)))));

        }else{
            float l=a.x<b.x?a.x:b.x;
            l=l<c.x?l:c.x;
            l=l<0?0:l;

            float r=a.x>b.x?a.x:b.x;
            r=r>c.x?r:c.x;
            r=r<0?0:r;

            float t=a.y<b.y?a.y:b.y;
            t=t<c.y?t:c.y;
            t=t<0?0:t;

            float bo=a.y>b.y?a.y:b.y;
            bo=bo>c.y?bo:c.y;
            bo=bo<0?0:bo;

            left = (int)floor(l);
            right=(int) ceil(r);
            right=right>width?width:right;
            top=(int) floor(t);
            bottom=(int) ceil(bo);
            bottom=bottom>height?height:bottom;

        }



        float S1=(-(a.x - b.x) * (c.y - b.y) + (a.y - b.y) * (c.x - b.x));
        float S2=(-(b.x - c.x) * (a.y - c.y) + (b.y - c.y) * (a.x - c.x));
        float dxa=-(c.y-b.y)/S1;
        float dya=(c.x-b.x)/S1;
        float dxb=-(a.y-c.y)/S2;
        float dyb=(a.x-c.x)/S2;
        float Ha1= (-(((float)left-0.5f) - b.x) * (c.y - b.y) + (((float)top-0.5f) - b.y) * (c.x - b.x))
                   / S1;
        float Hb1= (-(((float)left-0.5f) - c.x) * (a.y - c.y) + (((float)top-0.5f) - c.y) * (a.x - c.x))
                   / S2;
        for (int i=top;i<bottom;i++){
            Ha1+=dya;
            Hb1+=dyb;
            float ha=Ha1;
            float hb=Hb1;
            for (int j=left;j<right;j++){
                ha+=dxa;
                hb+=dxb;
                float hc=1-ha-hb;

                //if ha>0 && hb>0 && hc>0
                if (ha>=0 && hb>=0 && hc>=0){
                    float z=(a.z*ha+b.z*hb+c.z*hc); // 映射到0-1 值越大离相机越近
                    if(z<1 && z > zBuffer[i * width + j]){
                        zBuffer[i * width + j]=z;
                    }

                }
            }
        }
    }
}


//MSAAx4 采样
static void shadingS4(
        unsigned int width, unsigned int height, float *zBuffer,
        const unsigned int *indices,
        unsigned int indicesCount,
        const Vector4f *vertex) {

    auto count=indicesCount/3;
    for (auto index=0;index<count;index++){
        auto a=vertex[indices[index*3]];
        auto b=vertex[indices[index*3+1]];
        auto c=vertex[indices[index*3+2]];

        unsigned int left,right,top,bottom;
        {
            float l=a.x<b.x?a.x:b.x;
            l=l<c.x?l:c.x;
            l=l<0?0:l;

            float r=a.x>b.x?a.x:b.x;
            r=r>c.x?r:c.x;
            r=r<0?0:r;

            float t=a.y<b.y?a.y:b.y;
            t=t<c.y?t:c.y;
            t=t<0?0:t;

            float bo=a.y>b.y?a.y:b.y;
            bo=bo>c.y?bo:c.y;
            bo=bo<0?0:bo;

            left = (int)floor(l);
            right=(int) ceil(r);
            right=right>width?width:right;
            top=(int) floor(t);
            bottom=(int) ceil(bo);
            bottom=bottom>height?height:bottom;

        }



        float S1=(-(a.x - b.x) * (c.y - b.y) + (a.y - b.y) * (c.x - b.x));
        float S2=(-(b.x - c.x) * (a.y - c.y) + (b.y - c.y) * (a.x - c.x));
        float dxa=-(c.y-b.y)/S1;
        float dya=(c.x-b.x)/S1;
        float dxb=-(a.y-c.y)/S2;
        float dyb=(a.x-c.x)/S2;
        //在(0~1,0~1)范围内选择四个采样点
        //(0.625,0.125)
        //(0.875,0.625)
        //(0.375,0.875)
        //(0.125,0.375)

        float Ha1= (-(((float)left-1.0f) - b.x) * (c.y - b.y) + (((float)top-1.0f) - b.y) * (c.x - b.x))
                   / S1;
        float Hb1= (-(((float)left-1.0f) - c.x) * (a.y - c.y) + (((float)top-1.0f) - c.y) * (a.x - c.x))
                   / S2;

        float pointA[4],pointB[4];
        pointA[0]=Ha1+0.625f*dxa+0.125f*dya;
        pointB[0]=Hb1+0.625f*dxb+0.125f*dyb;

        pointA[1]=Ha1+0.875f*dxa+0.625f*dya;
        pointB[1]=Hb1+0.875f*dxb+0.625f*dyb;

        pointA[2]=Ha1+0.375f*dxa+0.875f*dya;
        pointB[2]=Hb1+0.375f*dxb+0.875f*dyb;

        pointA[3]=Ha1+0.125f*dxa+0.375f*dya;
        pointB[3]=Hb1+0.125f*dxb+0.375f*dyb;


        for (unsigned int i=top;i<bottom;i++){
            float ha[4],hb[4],hc[4];
            for(int k=0;k<4;k++){
                pointA[k]+=dya;
                pointB[k]+=dyb;
                ha[k]=pointA[k];
                hb[k]=pointB[k];
            }
            for (unsigned  j=left;j<right;j++){
                for(int k=0;k<4;k++){
                    ha[k]+=dxa;
                    hb[k]+=dxb;
                    hc[k]=1.0f-ha[k]-hb[k];

                    //if ha>0 && hb>0 && hc>0
                    if (ha[k]>=0 && hb[k]>=0 && hc[k]>=0){
                        float z=(a.z*ha[k]+b.z*hb[k]+c.z*hc[k]); // 映射到0-1 值越大离相机越近
                        if(z<1 && z > zBuffer[4 * (i * width + j) + k]){
                            zBuffer[4 * (i * width + j) + k]=z;
                        }

                    }

                }

            }
        }
    }
}




static void shading3(
        unsigned int width, unsigned int height, float *zbuffer,
        const unsigned int *indices,
        unsigned int indicesCount,
        const Vector4f *vertex) {

    auto count=indicesCount/3;
    for (auto index=0;index<count;index++){
        auto a=vertex[indices[index*3]];
        auto b=vertex[indices[index*3+1]];
        auto c=vertex[indices[index*3+2]];

        int left,right,top,bottom;

        if(false){
            left= (unsigned int) floor(fmax(0,fmin(a.x,fmin(b.x,c.x))));
            right=(unsigned int) ceil(fmin(width,fmax(0,fmax(a.x,fmax(b.x,c.x)))));
            top =(unsigned int) floor(fmax(0,fmin(a.y,fmin(b.y,c.y))));
            bottom=(unsigned int) ceil(fmin(height,fmax(0,fmax(a.y,fmax(b.y,c.y)))));

        }else{
            float l=a.x<b.x?a.x:b.x;
            l=l<c.x?l:c.x;
            l=l<0?0:l;

            float r=a.x>b.x?a.x:b.x;
            r=r>c.x?r:c.x;
            r=r<0?0:r;

            float t=a.y<b.y?a.y:b.y;
            t=t<c.y?t:c.y;
            t=t<0?0:t;

            float bo=a.y>b.y?a.y:b.y;
            bo=bo>c.y?bo:c.y;
            bo=bo<0?0:bo;

            left = (int)floor(l);
            right=(int) ceil(r);
            right=right>width?width:right;
            top=(int) floor(t);
            bottom=(int) ceil(bo);
            bottom=bottom>height?height:bottom;

        }


        //增量法计算三角形分量
        float S1=(-(a.x - b.x) * (c.y - b.y) + (a.y - b.y) * (c.x - b.x));
        float S2=(-(b.x - c.x) * (a.y - c.y) + (b.y - c.y) * (a.x - c.x));
        float dxa=-(c.y-b.y)/S1;
        float dya=(c.x-b.x)/S1;
        float dxb=-(a.y-c.y)/S2;
        float dyb=(a.x-c.x)/S2;
        float Ha1= (-(((float)left-0.5f) - b.x) * (c.y - b.y) + (((float)top+0.5f) - b.y) * (c.x - b.x))
                   / S1;
        float Hb1= (-(((float)left-0.5f) - c.x) * (a.y - c.y) + (((float)top+0.5f) - c.y) * (a.x - c.x))
                   / S2;

        float ha=Ha1;
        float hb=Hb1;
        float hc=0;
        bool direct= true;//true 向左 false 向右
        bool flag=false;
        int i=left;
        int j=top;
        while (j <bottom) {
            if (direct){
                ha+=dxa;
                hb+=dxb;
                hc=1-ha-hb;
                //if ha>0 && hb>0 && hc>0
                if (!((*(unsigned int*)&ha | *(unsigned int*)&hb | *(unsigned int*)&hc) & 0x80000000)){
                    float z=a.z*ha+b.z*hb+c.z*hc+1; // 映射到0-2 值越大离相机越近
                    if(z<2 && z > zbuffer[j*width+i]){
                        zbuffer[j*width+i]=z;
                    }
                    flag= true;
                }else{
                    if (flag) {
                        flag= false;
                        j++;
                        ha+=dya;
                        hb+=dyb;
                        ha-=dxa;
                        hb-=dya;
                        direct=!direct;
                        continue;
                    }
                }
                i++;
                if (i>=right){
                    i=right-1;
                    flag= false;
                    j++;
                    ha+=dya;
                    hb+=dyb;
                    ha-=dxa;
                    hb-=dxa;
                    direct=!direct;
                }
            }else{
                ha-=dxa;
                hb-=dxb;
                hc=1-ha-hb;
                //if ha>0 && hb>0 && hc>0
                if (ha >=0 && hb>=0 && hc>=0){
                    float z=a.z*ha+b.z*hb+c.z*hc+1; // 映射到0-2 值越大离相机越近
                    if(z<2 && z > zbuffer[j*width+i]){
                        zbuffer[j*width+i]=z;
                    }
                    flag= true;
                }else{
                    if (flag) {
                        flag= false;
                        j++;
                        ha+=dya;
                        hb+=dyb;
                        ha+=dxa;
                        hb+=dxa;
                        direct=!direct;
                        continue;
                    }
                }
                i--;
                if ( i<0 ){
                    i=0;
                    flag= false;
                    j++;
                    ha+=dya;
                    hb+=dyb;
                    ha+=dxa;
                    hb+=dxa;
                    direct=!direct;
                }
            }
        }


    }
}





void RasterizationShader::shading() {
    shading2(width,height,zBuffer,indices,indicesCount,vertex);
}

void RasterizationShader::shadingSimpling4() {
    shadingS4(width,height,zBuffer,indices,indicesCount,vertex);
}


