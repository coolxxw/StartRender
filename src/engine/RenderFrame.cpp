//
// Created by xxw on 2023/2/10.
//
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>

#include "RenderFrame.h"
#include "Util.h"

using namespace RenderCore;

RenderFrame::RenderFrame(Context *context) : context(context) {}

void RenderFrame::render() {
    ARGB* frameBuffer=context->frameBuffer;
    if(frameBuffer== nullptr){
        return;
    }
    refreshFrameBuffer();

    int width=context->width;
    int height=context->height;



    //初始化zbuffer
    refreshZBuffer();
    decimal *zBuffer=context->zBuffer;

    Vector3f lightL=-context->light.directionalLight;//光源
    Vector3f eye=-context->camera.g;


    //投影
    {
        Viewing viewing(context->camera,context->clipSpace,width,height);
        Matrix4f matrix;
        matrix.move(-context->object[0]->center);
        decimal scale=1/context->object[0]->objectSize;
        matrix.scale(Vector3f{scale,scale,scale});
        viewTransform(viewing.matrix*matrix);
    }


    for(Object3D* obj:context->object)
    {
        for(int faceIndex=0;faceIndex<obj->faceLength;faceIndex++)
        {
            //绘制一个三角形

            Object3D::Face face=obj->face[faceIndex];
            Vector4f* vertex=obj->viewVertex;

            Vector3f a=vertex[face.a].vec3();
            Vector3f b=vertex[face.b].vec3();
            Vector3f c=vertex[face.c].vec3();



            //表面颜色
            ARGB color=ARGB(100,100,100,255);

            //计算矩形边界
            int left= (int )fmax(0.0,floor(fmin(a.x,fmin(b.x,c.x))));
            int right= (int )fmin(width,ceil(fmax(a.x,fmax(b.x,c.x))));
            int top=  (int )fmax(0,floor(fmin(a.y,fmin(b.y,c.y))));
            int bottom=  (int )fmin(height,ceil(fmax(a.y,fmax(b.y,c.y))));

            //光删化
            rasterize(left,right,top,bottom,a.operator Vector2f(),b.operator Vector2f(),c.operator Vector2f());

            //遍历点
            for(int i=top;i<bottom;i++) {
                for (int j = left; j < right; j++) {
                    if(context->triangularComponentBuffer[4*(i*context->width+j)+3]<0){
                        continue;
                    }

                    //插值系数
                    decimal ha=context->triangularComponentBuffer[4*(i*context->width+j)+0];
                    decimal hb=context->triangularComponentBuffer[4*(i*context->width+j)+1];
                    decimal hc=context->triangularComponentBuffer[4*(i*context->width+j)+2];
                    decimal z;
                    //透视插值修正
                    {
                        decimal  n = context->clipSpace.n;
                        decimal  f = context->clipSpace.f;
                        //z=((n+f)-2nf/Z)/(n-f)
                        decimal  az=2*n*f/(a.z*(n-f)-(n+f));
                        decimal  bz=2*n*f/(b.z*(n-f)-(n+f));
                        decimal  cz=2*n*f/(c.z*(n-f)-(n+f));
                        z = 1 / (ha / az + hb / bz + hc / cz);
                        ha=ha*z/az;
                        hb=hb*z/bz;
                        hc=hc*z/cz;
                    }

                    if (z < -context->clipSpace.n || z > zBuffer[i * width + j]) {
                        continue;
                    }
                    zBuffer[i * width + j] = z;

                    //着色
                    //计算公式cr *(ca + cl*max (0, n · l)) + cl(h · n)^p
                    //插值法求出点法向量
                    Vector3f n=obj->normalVector[face.a]*ha+
                            obj->normalVector[face.b]*hb+
                            obj->normalVector[face.c]*hc;

                    n.normalization();

                    //漫反射系数
                    decimal diffuse=fmax(0.0,lightL*n);
                    decimal cr,cg,cb;
                    decimal al=context->light.ambient.a+diffuse*context->light.diffuse.a;
                    cr=(color.r*al);
                    cg=(color.g*al);
                    cb=(color.b*al);
                    //镜面光
                    //计算h=(e+l)/|e+l|
                    Vector3f h=(eye+lightL);
                    decimal hl= sqrt(h.x*h.x+h.y*h.y+h.z*h.z);
                    h=h*(1/hl);
                    decimal hn=h*n;
                    int pow=2;
                    while(pow<=context->light.specularExponent){
                        pow*=2;
                        hn*=hn;
                    }
                    hn=hn*context->light.specular.a;
                    cr+=context->light.specular.r*hn;
                    cg+=context->light.specular.g*hn;
                    cb+=context->light.specular.b*hn;
                    cr=cr>255?255:cr;
                    cg=cg>255?255:cg;
                    cb=cb>255?255:cb;


                    frameBuffer[i * width + j] =ARGB{(byte)cr,(byte)cg,(byte)cb,255};
                }
            }
        }
    }

}

void RenderFrame::refreshZBuffer() {
    for(int i=0;i<(context->width*context->height);i++)
    {
        context->zBuffer[i]=FLT_MAX;//无穷
    }
}

void RenderFrame::refreshFrameBuffer() {
    ARGB* frameBuffer=context->frameBuffer;
    for(int i=0;i<(context->width*context->height);i++)
    {
        frameBuffer[i]=ARGB(127,127,127,255);
    }
}

inline void RenderFrame::rasterize(int left, int right, int top, int bottom, Vector2f a, Vector2f b, Vector2f c) {
    decimal* buffer=context->triangularComponentBuffer;
    int width=context->width;
    decimal S1=(-(a.x - b.x) * (c.y - b.y) + (a.y - b.y) * (c.x - b.x));
    decimal S2=(-(b.x - c.x) * (a.y - c.y) + (b.y - c.y) * (a.x - c.x));

#define RasterizeEnableSIMD

#ifdef RasterizeEnableSIMD
    __m256d m1 = _mm256_set_pd(b.y,c.y,b.x,c.x);
    __m256d m2 = _mm256_set_pd(c.y,a.y,c.x,a.x);
    m1= _mm256_sub_pd(m2,m1);//(cy-by) (ay-cy) (cx-bx) (ax-cx)
    m2 = _mm256_set_pd(b.x,c.x,b.y,c.y);
    __m128d s= _mm_set_pd(S1,S2);

    for(int i=top;i<bottom;i++) {
        decimal x = (decimal) left - 0.5;
        decimal y = (decimal) i + 0.5;
        __m256d m3 = _mm256_set_pd(x,x,y,y);
        __m256d m4 = _mm256_set_pd(1,1,0,0);
        for (int j = left; j < right; j++) {
            m3= _mm256_add_pd(m3,m4);
            __m256d m5 = _mm256_sub_pd(m3,m2);//(x-bx) (x-cx) (y-by) (y-cy)
            m5 = _mm256_mul_pd(m5, m1);//(x-bx)(cy-by) (x-cx)(ay-cy) (y-by)(cx-bx) (y-cy)(ax-cx)
            __m128d m6,m7;
            _mm256_storeu2_m128i((__m128i*)&m6,(__m128i*)&m7,*(__m256i*)&m5);
            m6= _mm_sub_pd(m7,m6);
            m6= _mm_div_pd(m6,s);
            decimal v[2];
            _mm_storeu_pd(v,m6);
            decimal ha = v[1];
            decimal hb = v[0];
            decimal hc = 1 - ha - hb;

            if (ha >= 0 && hb >= 0 && hc >= 0) {
                buffer[4 * (i * width + j) + 0] = ha;
                buffer[4 * (i * width + j) + 1] = hb;
                buffer[4 * (i * width + j) + 2] = hc;
                buffer[4 * (i * width + j) + 3] = 1;
            } else {
                buffer[4 * (i * width + j) + 3] = -1;
            }
        }
    }

#else
    for(int i=top;i<bottom;i++){
        for(int j=left;j<right;j++){
            decimal x=(decimal)j+0.5;
            decimal y=(decimal)i+0.5;
            //计算分量

            decimal ha = (-(x - b.x) * (c.y - b.y) + (y - b.y) * (c.x - b.x))
                    / S1;
            decimal hb = (-(x - c.x) * (a.y - c.y) + (y - c.y) * (a.x - c.x))
                    / S2;
            decimal hc=1-ha-hb;

             if(ha>0 && hb>0 && hc>0 ){
                buffer[4*(i*width+j)+0]=ha;
                buffer[4*(i*width+j)+1]=hb;
                buffer[4*(i*width+j)+2]=hc;
                buffer[4*(i*width+j)+3]=1;
            }else{
                buffer[4*(i*width+j)+3]=-1;
            }
        }
    }

#endif

}

void RenderFrame::viewTransform(Matrix4f matrix) {

    decimal (*m)[4]=matrix.m;
    __m256d matrix1= _mm256_set_pd(m[3][0],m[2][0],m[1][0],m[0][0]);
    __m256d matrix2= _mm256_set_pd(m[3][1],m[2][1],m[1][1],m[0][1]);
    __m256d matrix3= _mm256_set_pd(m[3][2],m[2][2],m[1][2],m[0][2]);
    __m256d matrix4= _mm256_set_pd(m[3][3],m[2][3],m[1][3],m[0][3]);

    for(auto obj:context->object){
        for(int i=0;i<obj->vertexLength;i++){
            Vector4f vector=obj->vertex[i];
           __m256d vec,v1,v2;
           vec = _mm256_set1_pd(vector.x);
           v1= _mm256_mul_pd(matrix1,vec);

           vec = _mm256_set1_pd(vector.y);
           v2= _mm256_mul_pd(matrix2,vec);
           v1= _mm256_add_pd(v1,v2);

           vec = _mm256_set1_pd(vector.z);
            v2= _mm256_mul_pd(matrix3,vec);
            v1= _mm256_add_pd(v1,v2);

            vec = _mm256_set1_pd(vector.w);
            v2= _mm256_mul_pd(matrix4,vec);
            v1= _mm256_add_pd(v1,v2);

            decimal d[4];
            _mm256_store_pd(d,v1);
            vec = _mm256_set1_pd(d[3]);
            v1= _mm256_div_pd(v1,vec);

            _mm256_store_pd((double *)&(obj->viewVertex[i]),v1);
            ;

        }
    }

    return;
    for(auto obj:context->object){
        for(int i=0;i<obj->vertexLength;i++){
            Vector4f v=matrix*obj->vertex[i];
            obj->viewVertex[i]=v*(1/v.w);
        }
    }

    return;




}




