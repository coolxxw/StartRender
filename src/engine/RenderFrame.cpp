//
// Created by xxw on 2023/2/10.
//
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>

#include "RenderFrame.h"
#include "Util.h"

RenderFrame::RenderFrame(Context *context) : context(context) {}

void RenderFrame::render() {
    ARGB* frameBuffer=context->frameBuffer;
    if(frameBuffer== nullptr){
        return;
    }
    refreshFrameBuffer();

    int width=context->width;
    int height=context->height;

    //投影
    Viewing viewing(context->camera,context->clipSpace,width,height);

    //初始化zbuffer
    refreshZBuffer();
    float *zBuffer=context->zBuffer;

    Vector3f lightL=-context->light.directionalLight;//光源
    Vector3f eye=-context->camera.g;

    srand(1);

    for(Object3D* obj:context->object)
    {
        for(int faceIndex=0;faceIndex<obj->faceLength;faceIndex++)
        {
            //绘制一个三角形


            Object3D::Face face=obj->face[faceIndex];
            Object3D::Vertex* vertex=obj->vertex;

            Object3D::Vertex vertexA=vertex[face.a];
            Object3D::Vertex vertexB=vertex[face.b];
            Object3D::Vertex vertexC=vertex[face.c];
            Vector3f move=obj->center;
            Vector3f objA=vertexA.toVector3f();
            Vector3f objB=vertexB.toVector3f();
            Vector3f objC=vertexC.toVector3f();
            objA=objA-move;
            objB=objB-move;
            objC=objC-move;
            objA=objA*(1/obj->objectSize);
            objB=objB*(1/obj->objectSize);
            objC=objC*(1/obj->objectSize);


            //投影
            Vector3f a=viewing.projection(objA);
            Vector3f b=viewing.projection(objB);
            Vector3f c=viewing.projection(objC);


            //表面颜色
            ARGB color=ARGB(100,100,100,255);

            //计算矩形边界
            int left= fmax(0.0,floor(fmin(a.x,fmin(b.x,c.x))));
            int right= fmin(width,ceil(fmax(a.x,fmax(b.x,c.x))));
            int top=  fmax(0,floor(fmin(a.y,fmin(b.y,c.y))));
            int bottom=  fmin(height,ceil(fmax(a.y,fmax(b.y,c.y))));

            //遍历点
            for(int i=top;i<bottom;i++) {
                for (int j = left; j < right; j++) {
                    //像素点中心坐标
                    Vector2f point = Vector2f((float) j + 0.5, (float) i + 0.5);
                    //插值系数
                    float ha;
                    float hb;
                    float hc;
                    float z;
                    //顶点

                    //判断是否在三角形内
                    if (Util::isPointInTrangle(point, a, b, c)) {
                        bool zbufferFlag = false;
                        //计算zbuffer的值
                        {
                            float x = point.x;
                            float y = point.y;
                            //计算分量
                            ha = (-(x - b.x) * (c.y - b.y) + (y - b.y) * (c.x - b.x))
                                       / (-(a.x - b.x) * (c.y - b.y) + (a.y - b.y) * (c.x - b.x));
                            hb = (-(x - c.x) * (a.y - c.y) + (y - c.y) * (a.x - c.x))
                                       / (-(b.x - c.x) * (a.y - c.y) + (b.y - c.y) * (a.x - c.x));
                            hc =  1-ha-hb;
                            //透视插值修正
                            float n = context->clipSpace.n;
                            float f = context->clipSpace.f;
                            //z=((n+f)-2nf/Z)/(n-f)
                            float limit=((f+n) / (n - f));
                            {
                                float az=2*n*f/(a.z*(n-f)-(n+f));
                                float bz=2*n*f/(b.z*(n-f)-(n+f));
                                float cz=2*n*f/(c.z*(n-f)-(n+f));
                                z = 1 / (ha / az + hb / bz + hc / cz);
                                ha=ha*z/az;
                                hb=hb*z/bz;
                                hc=hc*z/cz;
                            }


                            if (z > -n && z < zBuffer[i * width + j]) {
                                zBuffer[i * width + j] = z;
                                zbufferFlag = true;
                            }
                        }//zbuffer

                        if (!zbufferFlag) {
                            continue;
                        }

                        //着色
                        //计算公式cr *(ca + cl*max (0, n · l)) + cl(h · n)^p
                        //插值法求出点法向量
                        Vector3f n=vertexA.normalVector*ha+
                                vertexB.normalVector*hb+
                                vertexC.normalVector*hc;

                        n.normalization();

                        //漫反射系数
                        Vector3f n2=vertexA.normalVector+vertexC.normalVector+vertexB.normalVector;
                        n2.normalization();
                        float diffuse=fmax(0.0,lightL*n);
                        int cr,cg,cb;
                        float al=context->light.ambient.a+diffuse*context->light.diffuse.a;
                        cr=color.r*al;
                        cg=color.g*al;
                        cb=color.b*al;
                        //镜面光
                        //计算h=(e+l)/|e+l|
                        Vector3f h=(eye+lightL);
                        float hl= sqrt(h.x*h.x+h.y*h.y+h.z*h.z);
                        h=h*(1/hl);
                        float hn=h*n;
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


