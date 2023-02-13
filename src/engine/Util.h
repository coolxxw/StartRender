//
// Created by xxw on 2022/11/10.
//

#ifndef STARTRENDER_UTIL_H
#define STARTRENDER_UTIL_H

#include "../math/Vector.h"

class Util {
public:
    static bool isPointInTrangle(Vector2f p,Vector2f a,Vector2f b,Vector2f c){
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

};


#endif //STARTRENDER_UTIL_H
