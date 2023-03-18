//
// Created by xxw on 2022/11/11.
//

#ifndef STARTRENDERCORE_CAMERA_H
#define STARTRENDERCORE_CAMERA_H

#include "../../math/Vector.h"


namespace render_core{

//相机
    class Camera {
    public:
        Vector3f e;
        Vector3f t;
        Vector3f g;


        Camera(): e(0, 0, 1.5),
        t(0,1,0),
        g(0, 0, -1) {

        }
    };
}

#endif //STARTRENDERCORE_CAMERA_H
