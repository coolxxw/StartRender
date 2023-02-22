//
// Created by xxw on 2023/2/19.
//

#ifndef STARTRENDER_OBJECTOPERATOR_H
#define STARTRENDER_OBJECTOPERATOR_H

#include "Object3D.h"

class ObjectOperator {
private:
    Object3D* object;

public:
    explicit ObjectOperator(Object3D* object):object(object){}

    //计算出顶点法向量
    void calculateNormalVector();
    void calculateCenterAndSize();


};


#endif //STARTRENDER_OBJECTOPERATOR_H
