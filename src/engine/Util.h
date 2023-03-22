//
// Created by xxw on 2022/11/10.
//

#ifndef STARTRENDER_UTIL_H
#define STARTRENDER_UTIL_H


#include <atomic>
#include "../math/Vector.h"
#include "../engine/platform/SIMDUtil.h"

class Util {
public:


    static bool isPointInTrange(Vector2f p, Vector2f a, Vector2f b, Vector2f c){


        return isPointInTrangeSimd(p,a,b,c);


        Vector2f ap;
        ap.x=p.x-a.x;
        ap.y=p.y-a.y;//向量ap=op-oa;
        Vector2f ab;
        ab.x=b.x-a.x;
        ab.y=b.y-a.y;
        Vector2f bp;
        bp.x=p.x-b.x;
        bp.y=p.y-b.y;
        Vector2f bc;
        bc.x=c.x-b.x;
        bc.y=c.y-b.y;

        float p1=ap.cross(ab);//ap和ab的差积
        float p2=bp.cross(bc);
        if(p1*p2<0){
            return false;
        }
        Vector2f cp=p-c;
        Vector2f ca=a-c;
        float p3=cp.cross(ca);
        if(p3*p2<0){
            return false;
        }
        return true;
    }

    static inline bool isPointInTrangeSimd(Vector2f p, Vector2f a, Vector2f b, Vector2f c){

#ifdef USE_DECIMAL_FLOAT64

        //ap ab bp bc cp ca
        double num[]={p.x,p.y,a.x,a.y,b.x,b.y,c.x,c.y};
        __m256d m1 = _mm256_set_pd(a.x,b.x,a.y,b.y);
        __m256d m2 = _mm256_set_pd(p.x,p.x,p.y,p.y);
        __m256d m3 = _mm256_set_pd(a.y,b.y,a.x,b.x);
        __m256d m4 = _mm256_set_pd(b.y,c.y,b.x,c.x);
        m2= _mm256_sub_pd(m2,m1);//(x-ax) (x-bx) (y-ay) (y-by)
        m4= _mm256_sub_pd(m4,m3);//(by-ay) (cy-by) (bx-ax) (cx-bx)
        m2= _mm256_mul_pd(m2,m4);//(x-ax)(by-ay) (x-bx)(cy-by) (y-ay)(bx-ax) (y-by)(cx-bx)
        __m128d m5;
        __m128d m6;
        _mm256_storeu2_m128i((__m128i*)&m5,(__m128i*)&m6,*(__m256i*)&m2);
        m5= _mm_sub_pd(m5,m6);
        double v[4];
        _mm_store_pd(v,m5);
        if(v[0]*v[1]<0) {
            return false;
        }
        m1 = _mm256_set_pd(p.x,p.y,a.y,a.x);
        m2 = _mm256_set_pd(c.x,c.y,c.y,c.x);
        m1= _mm256_sub_pd(m1,m2);//(x-cx) (y-cy)  (ay-cy) (ax-cx)
        _mm256_storeu2_m128i((__m128i*)&m5,(__m128i*)&m6,*(__m256i*)&m1);
        m5= _mm_mul_pd(m5,m6);//(x-cx)(ay-cy) (y-cy)(ax-cx)
        _mm_store_pd(v+2,m5);
        if((v[3]-v[2])*v[0]<0) {
            return false;
        }
        return true;

    }

#else
Vector2f ap;
        ap.x=p.x-a.x;
        ap.y=p.y-a.y;//向量ap=op-oa;
        Vector2f ab;
        ab.x=b.x-a.x;
        ab.y=b.y-a.y;
        Vector2f bp;
        bp.x=p.x-b.x;
        bp.y=p.y-b.y;
        Vector2f bc;
        bc.x=c.x-b.x;
        bc.y=c.y-b.y;

        float p1=ap.cross(ab);//ap和ab的差积
        float p2=bp.cross(bc);
        if(p1*p2<0){
            return false;
        }
        Vector2f cp=p-c;
        Vector2f ca=a-c;
        float p3=cp.cross(ca);
        if(p3*p2<0){
            return false;
        }
        return true;
    }

#endif



};



class InOutLock{
private:
    volatile bool in= false;
    volatile bool out= false;

public:

    void inLock(){
        while(true){
            while(out);
            in=true;
            if(out){
                in=false;
            }else{
                break;
            }
        }
    }

    void inUnlock(){
        in= false;
    }

    void outLock(){
        while(true){
            while(in);
            out=true;
            if(in){
                out=false;
            }else{
                break;
            }
        }
    }
    void outUnlock(){
        out=false;
    }



};

namespace render{

    class CAS{
        std::atomic<bool> flag;



    public:
        void lock(){
            bool expect = false;
            while (!flag.compare_exchange_strong(expect, true))
            {
                expect = false;
            }
        }

        void unlock(){
            flag.store(false);
        }
    };


}


#endif //STARTRENDER_UTIL_H
