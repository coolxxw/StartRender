//
// Created by xxw on 2023/3/18.
//

#ifndef STARTRENDER_CONTEXTCACHE_H
#define STARTRENDER_CONTEXTCACHE_H

#include "Camera.h"
#include "Environment.h"

namespace render_core{
    class ContextCache{
    public:
        Camera camera;
        int width;
        int height;
        Environment light;
    };

}



#endif //STARTRENDER_CONTEXTCACHE_H
