//
// Created by xxw on 2023/3/12.
//

#ifndef STARTRENDER_GBUFFER_H
#define STARTRENDER_GBUFFER_H

#include <atomic>
#include <vector>
#include "../../math/Vector.h"

namespace render_core{

    enum GBufferFlag{
        None=0,//无采样
        Sampling,//正常采样
        Sampling4,//四倍采样
    };

    struct GBufferUnit{
        GBufferFlag flag;
        union{
            unsigned int index[4]{};//如果flag代表四倍采样则index记录四个gbuffer索引
            struct{
                Vector3f normal;
                RGBAF baseColor;//基础颜色或反射率
                float metallic;//金属度
                float roughness;//主观粗糙度
                RGBAF emission;//自发光

            };
        };

        GBufferUnit(){}

    };


    class GBufferCache{
    public:
        std::vector<GBufferUnit> cache;
        int count=0;
        std::atomic<bool> flag;

        GBufferCache(){
            cache.reserve(4096);
        }

        void clear(){
            bool expect = false;
            while (!flag.compare_exchange_strong(expect, true))
            {
                expect = false;
            }

            cache.clear();
            count=1;

            flag.store(false);
        }

        GBufferUnit* get(unsigned index){
            return &cache[index];
        }

        unsigned int getIndex(){
            bool expect = false;
            while (!flag.compare_exchange_strong(expect, true))
            {
                expect = false;
            }

            int r=count++;
            if(cache.size()<count){
                cache.resize(count);
            }

            flag.store(false);

            return r;
        }


    };



}




#endif //STARTRENDER_GBUFFER_H
