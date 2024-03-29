//
// Created by xxw on 2023/3/18.
//

#ifndef STARTRENDER_CONTEXTCACHE_H
#define STARTRENDER_CONTEXTCACHE_H

#include "Camera.h"
#include "Environment.h"
#include "Config.h"

namespace render{

    class ContextCache{

    public:
        int width=0;
        int height=0;
        Environment light;
        Camera camera;
        Config config;
    };

}



#endif //STARTRENDER_CONTEXTCACHE_H
