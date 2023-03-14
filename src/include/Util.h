//
// Created by xxw on 2023/2/22.
//

#ifndef STARTRENDER_UTIL_H
#define STARTRENDER_UTIL_H

#include "../math/Vector.h"

namespace render{
    namespace Util{

        /**
         * 将单位向量转为角度
         * @param vector
         * @param angle_h 0.0~360.0f 向量在xoy平面投影的角度，x轴正反向为起始点，逆时针反向
         * @param angle_v -90~90.0f 向量在yoz平面投影的角度，y轴正反向为起始点，从-x反向看逆时针反向
         */
        void VectorToAngle(Vector3f vector,float *angle_h,float *angle_v);

        /**
         * 参考 VectorToAngle(Vector3f vector,float *angle_h,float *angle_v);
         * @param angle_h
         * @param angle_v
         * @return
         */
        Vector3f AngleToVector(float angle_h,float angle_v);


    }
}


#endif //STARTRENDER_UTIL_H
