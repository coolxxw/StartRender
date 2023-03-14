//
// Created by xxw on 2023/3/12.
//

#include "FragmentShader.h"

using namespace render_core;

void FragmentShader::shading(
        unsigned int width, unsigned int height, const float *zbuffer,
        const unsigned int *indices, unsigned int indicesCount,
        const Vector4f *vertex,
        const Vector4f* preVertex,
        const VertexAttribute  *attribute,//2*width*height
        render_core::Texture normalTexture,
        render_core::Texture baseColorTexture,
        render_core::GBufferUnit *gBuffer) {

    auto count=indicesCount/3;
    for (auto index=0;index<count;index++){


        auto ai=indices[index*3];
        auto bi=indices[index*3+1];
        auto ci=indices[index*3+2];
        auto a=preVertex[ai];
        auto b=preVertex[bi];
        auto c=preVertex[ci];
        //计算法线b
        Vector3f AC= c.vec3() - a.vec3();
        Vector3f AB= b.vec3() - a.vec3();


        //计算uv坐标
        auto aNormal=attribute[ai].normal;
        auto ua=attribute[ai].u;
        auto va=attribute[ai].v;
        auto bNormal=attribute[bi].normal;
        auto ub=attribute[bi].u;
        auto vb=attribute[bi].v;
        auto cNormal=attribute[ci].normal;
        auto uc=attribute[ci].u;
        auto vc=attribute[ci].v;



        //TBN坐标系
        //向量AC=x1*t+y1*b;
        //向量AB=x2*t+y2*b;
        //[AB AC]=[t b][(x1,y1) (x2,y2)]
        auto x1=uc-ua;
        auto y1=(vc-va);
        auto x2=ub-ua;
        auto y2=(vb-va);
        //[t b]=[AC AB][(x1,y1) (x2,y2)]逆
        //矩阵 |x1 x2| 逆为 | y2 -x2 |/行列式
        //     |y1 y2|      |-y1 x1 |
        auto det=x1*y2-x2*y1;
        x1/=-det;
        x2/=-det;
        y1/=-det;
        y2/=-det;
        auto T= AC * y2 - AB * y1;
        auto B= AC * (-x2) + AB * x1;
        //auto N=T.cross(B);
        auto N=-AC.cross(AB);

        T.normalization();
        B.normalization();
        N.normalization();



        a=vertex[ai];
        b=vertex[bi];
        c=vertex[ci];

        int left,right,top,bottom;

        {
            float l=a.x<b.x?a.x:b.x;
            l=l<c.x?l:c.x;
            l=l<0?0:l;

            float r=a.x>b.x?a.x:b.x;
            r=r>c.x?r:c.x;
            r=r<0?0:r;

            float t=a.y<b.y?a.y:b.y;
            t=t<c.y?t:c.y;
            t=t<0?0:t;

            float bo=a.y>b.y?a.y:b.y;
            bo=bo>c.y?bo:c.y;
            bo=bo<0?0:bo;

            left = (int)floor(l);
            right=(int) ceil(r);
            right=right>width?width:right;
            top=(int) floor(t);
            bottom=(int) ceil(bo);
            bottom=bottom>height?height:bottom;

        }



        float S1=(-(a.x - b.x) * (c.y - b.y) + (a.y - b.y) * (c.x - b.x));
        float S2=(-(b.x - c.x) * (a.y - c.y) + (b.y - c.y) * (a.x - c.x));
        float dxa=-(c.y-b.y)/S1;
        float dya=(c.x-b.x)/S1;
        float dxb=-(a.y-c.y)/S2;
        float dyb=(a.x-c.x)/S2;
        float Ha1= (-(((float)left-0.5f) - b.x) * (c.y - b.y) + (((float)top-0.5f) - b.y) * (c.x - b.x))
                   / S1;
        float Hb1= (-(((float)left-0.5f) - c.x) * (a.y - c.y) + (((float)top-0.5f) - c.y) * (a.x - c.x))
                   / S2;
        for (int i=top;i<bottom;i++){
            Ha1+=dya;
            Hb1+=dyb;
            float ha=Ha1;
            float hb=Hb1;
            for (int j=left;j<right;j++){
                ha+=dxa;
                hb+=dxb;
                float hc=1-ha-hb;
                if(! (ha>=0 || hb>=0 || hc>=0)){
                    continue;
                }


                float z=a.z*ha+b.z*hb+c.z*hc; // 映射到0-2 值越大离相机越近
                if(z == zbuffer[i*width+j]){
                    //通过z缓存测试
                    //开始着色
                    float fixHa= ha * a.z / z;
                    float fixHb= hb * b.z / z;
                    float fixHc= hc * c.z / z;

                    auto u= ua * fixHa + ub * fixHb + uc * fixHc;
                    auto v= va * fixHa + vb * fixHb + vc * fixHc;

                    auto nrgb=normalTexture.getAttribute(u,v);
                    //插值顶点法向量
                    auto vNormal= (aNormal * fixHa) + (bNormal * fixHb) + (cNormal * fixHc);
                    vNormal.normalization();
                    //查询法向量贴图
                    N=vNormal;
                    auto tNormal=T*(float )(nrgb.r-0.5)+B*(float )(nrgb.g-0.5)+N*(float )(nrgb.b-0.5);
                    tNormal.normalization();

                    //叠加法向量
                    gBuffer[i*width+j].normal=tNormal;

                    gBuffer[i*width+j].normal.normalization();


                    gBuffer[i*width+j].baseColor=baseColorTexture.getAttribute(u,v);



                    //结束着色
                }
            }
        }
    }


}
