//
// Created by xxw on 2022/11/11.
//

#ifndef STARTRENDER_VIEWING_H
#define STARTRENDER_VIEWING_H

#include <cstdlib>
#include "Camera.h"
#include "../math/Matrix.h"

namespace RenderCore{

//裁剪空间
    class ClipSpace{
    public:
        float n;
        float f;
        float l;
        float r;
        float t;
        float b;

        ClipSpace(){
            //设n=1;
            n=-0.2;
            f=-4;
            r=1;
            t=1;
            l=0;
            b=0;
            setHFieldView(3.1415923/3);
        }

        //调整长宽比
        void setAspect(float aspect){
            t=r/aspect;
            b=-t;
        }

        //调整水平可视角度
        void setHFieldView(float horizontalFieldView){
            float aspect=r/t;
            r=-n*tan(horizontalFieldView/2);
            l=-r;
            t=r/aspect;
            b=-t;
        }


    };


//投影变换
    class Viewing {
    public:
        Matrix4f cam;
        Matrix4f matrix;


    public:
        Viewing(const RenderCore::Camera &camera, ClipSpace clipSpace, int width, int height);
        Vector3f projection(Vector3f);
        Vector3f cameraTranform(Vector3f);

    };


}


#endif //STARTRENDER_VIEWING_H
