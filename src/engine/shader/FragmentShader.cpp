//
// Created by xxw on 2023/3/12.
//
#include <assert.h>
#include "FragmentShader.h"

using namespace render_core;

//根据ABC三点计算TBN坐标系 不要交换ABC三点顺序
static inline void TbnTrans(Vector3f &AC,Vector3f &AB,
                            float x1,float y1,float x2,float y2,
                            Vector3f &T,Vector3f &B,Vector3f &N){
    //TBN坐标系
    //向量AC=x1*t+y1*b;
    //向量AB=x2*t+y2*b;
    //[AB AC]=[t b][(x1,y1) (x2,y2)]
    //[t b]=[AC AB][(x1,y1) (x2,y2)]逆
    //矩阵 |x1 x2| 逆为 | y2 -x2 |/行列式
    //     |y1 y2|      |-y1 x1 |
    auto det=x1*y2-x2*y1;
    x1/=-det;
    x2/=-det;
    y1/=-det;
    y2/=-det;
    T= AC * y2 - AB * y1;
    B= AC * (-x2) + AB * x1;
    //auto N=T.cross(B);
    N=-AC.cross(AB);

    T.normalization();
    B.normalization();
    N.normalization();

}

//计算三角形abc边界 剔除不在屏幕范围内的区域
static inline void boundaryCulling(
        const Vector4f &a,const Vector4f &b,const Vector4f &c,
        unsigned int width,unsigned int height,
        unsigned int &left,unsigned int &right,unsigned int &top,unsigned int &bottom
        ){
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

static inline void setGBuffer(GBufferUnit *gBuffer,const Object* object,float u,float v,
                              const Vector3f &vNormal,
                              const Vector3f&T,const Vector3f&B,const Vector3f &N2){
    auto nrgb=object->normalTexture.getAttribute(u,v);

    //查询法向量贴图
    Vector3f N=vNormal;
    auto tNormal=T*(float )(nrgb.r-0.5)+B*(float )(nrgb.g-0.5)+N*(float )(nrgb.b-0.5);
    tNormal.normalization();

    //叠加法向量
    gBuffer->normal=tNormal;

    gBuffer->normal.normalization();

    gBuffer->baseColor=object->baseColorTexture.getAttribute(u,v);

    //金属粗糙度
    auto metalRoughness=object->metalRoughnessTexture.getAttribute(u,v);
    gBuffer->metallic=metalRoughness.b;
    gBuffer->roughness=metalRoughness.g;

    //自发光
    auto emission=object->emissionTexture.getAttribute(u,v);
    gBuffer->emission=emission;


}


void FragmentShader::shadingNoAnti() {

    const unsigned int *indices=object->indices;
    unsigned int indicesCount=object->indicesCount;
    const Vector4f *vertex=object->vertices;
    const Vector4f* preVertex=object->preVertices;
    const VertexAttribute  *attribute=object->attribute;//2*width*height
    TextureMap normalTexture=object->normalTexture;
    TextureMap baseColorTexture=object->baseColorTexture;
    TextureMap metalRoughnessTexture=object->metalRoughnessTexture;
    TextureMap emissionTexture=object->emissionTexture;


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


        //切线空间转换TBN坐标系
        Vector3f T,B,N;
        TbnTrans(AC,AB,uc-ua,vc-va,ub-ua,vb-va,T,B,N);


        a=vertex[ai];
        b=vertex[bi];
        c=vertex[ci];

        //计算边界
        unsigned int left,right,top,bottom;
        boundaryCulling(a,b,c,width,height,left,right,top,bottom);


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
        for (unsigned int i=top;i<bottom;i++){
            Ha1+=dya;
            Hb1+=dyb;
            float ha=Ha1;
            float hb=Hb1;
            for (unsigned int j=left;j<right;j++){
                ha+=dxa;
                hb+=dxb;
                float hc=1-ha-hb;
                if(! (ha>=0 || hb>=0 || hc>=0)){
                    continue;
                }


                float z=a.z*ha+b.z*hb+c.z*hc; // 映射到0-1 值越大离相机越近
                if(z == zBuffer[i*width+j]){
                    //通过z缓存测试
                    //开始着色

                    gBuffer[i*width+j].flag=GBufferFlag::Sampling;

                    float fixHa= ha * a.z / z;
                    float fixHb= hb * b.z / z;
                    float fixHc= hc * c.z / z;

                    auto u= ua * fixHa + ub * fixHb + uc * fixHc;
                    auto v= va * fixHa + vb * fixHb + vc * fixHc;

                    //插值顶点法向量
                    auto vNormal= (aNormal * fixHa) + (bNormal * fixHb) + (cNormal * fixHc);
                    vNormal.normalization();

                    setGBuffer(&(gBuffer[i*width+j]),object,u,v,vNormal,T,B,N);


                    //结束着色
                }
            }
        }
    }
}


//MSAAx4 多重采样计算gBuffer
void FragmentShader::shadingMSAAx4() {
    const unsigned int *indices=object->indices;
    unsigned int indicesCount=object->indicesCount;
    const Vector4f *vertex=object->vertices;
    const Vector4f* preVertex=object->preVertices;
    const VertexAttribute  *attribute=object->attribute;//2*width*height


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


        //切线空间转换TBN坐标系
        Vector3f T,B,N;
        TbnTrans(AC,AB,uc-ua,vc-va,ub-ua,vb-va,T,B,N);


        a=vertex[ai];
        b=vertex[bi];
        c=vertex[ci];

        //计算边界
        unsigned int left,right,top,bottom;
        boundaryCulling(a,b,c,width,height,left,right,top,bottom);


        float S1=(-(a.x - b.x) * (c.y - b.y) + (a.y - b.y) * (c.x - b.x));
        float S2=(-(b.x - c.x) * (a.y - c.y) + (b.y - c.y) * (a.x - c.x));
        float dxa=-(c.y-b.y)/S1;
        float dya=(c.x-b.x)/S1;
        float dxb=-(a.y-c.y)/S2;
        float dyb=(a.x-c.x)/S2;
        //在(0~1,0~1)范围内选择四个采样点
        //(0.625,0.125)
        //(0.875,0.625)
        //(0.375,0.875)
        //(0.125,0.375)

        float Ha1= (-(((float)left-1.0f) - b.x) * (c.y - b.y) + (((float)top-1.0f) - b.y) * (c.x - b.x))
                   / S1;
        float Hb1= (-(((float)left-1.0f) - c.x) * (a.y - c.y) + (((float)top-1.0f) - c.y) * (a.x - c.x))
                   / S2;
        //四个采样点
        float pointA[4],pointB[4];
        pointA[0]=Ha1+0.625f*dxa+0.125f*dya;
        pointB[0]=Hb1+0.625f*dxb+0.125f*dyb;

        pointA[1]=Ha1+0.875f*dxa+0.625f*dya;
        pointB[1]=Hb1+0.875f*dxb+0.625f*dyb;

        pointA[2]=Ha1+0.375f*dxa+0.875f*dya;
        pointB[2]=Hb1+0.375f*dxb+0.875f*dyb;

        pointA[3]=Ha1+0.125f*dxa+0.375f*dya;
        pointB[3]=Hb1+0.125f*dxb+0.375f*dyb;


        for (unsigned int i=top;i<bottom;i++){
            float ha[4],hb[4],hc[4];
            for(int k=0;k<4;k++){
                pointA[k]+=dya;
                pointB[k]+=dyb;
                ha[k]=pointA[k];
                hb[k]=pointB[k];
            }
            float z[4];
            for (unsigned  j=left;j<right;j++){



                for(int k=0;k<4;k++){
                    ha[k]+=dxa;
                    hb[k]+=dxb;
                    hc[k]=1.0f-ha[k]-hb[k];
                    z[k]=-1;
                    if((ha[k]>0)&& (hb[k]>0)&& (hc[k]>0)){
                        z[k]=a.z*ha[k]+b.z*hb[k]+c.z*hc[k];
                    }
                }

                if(   (z[0]==zBuffer[4*(i*width+j)+0])
                    &&(z[1]==zBuffer[4*(i*width+j)+1])
                    &&(z[2]==zBuffer[4*(i*width+j)+2])
                    &&(z[3]==zBuffer[4*(i*width+j)+3])
                ){
                    //四个点均通过测试
                    //通过z缓存测试
                    //开始着色

                    gBuffer[i*width+j].flag=GBufferFlag::Sampling;

                    float ha_ave=(ha[0]+ha[1]+ha[2]+ha[3])/4;
                    float hb_ave=(hb[0]+hb[1]+hb[2]+hb[3])/4;
                    float hc_ave=(hc[0]+hc[1]+hc[2]+hc[3])/4;
                    float z_ave=a.z*ha_ave+b.z*hb_ave+c.z*hc_ave;

                    float fixHa= ha_ave * a.z / z_ave;
                    float fixHb= hb_ave * b.z / z_ave;
                    float fixHc= hc_ave * c.z / z_ave;

                    auto u= ua * fixHa + ub * fixHb + uc * fixHc;
                    auto v= va * fixHa + vb * fixHb + vc * fixHc;


                    //插值顶点法向量
                    auto vNormal= (aNormal * fixHa) + (bNormal * fixHb) + (cNormal * fixHc);
                    vNormal.normalization();


                    setGBuffer(&(gBuffer[i*width+j]),object,u,v,vNormal,T,B,N);

                    //结束着色

                }else{
                    if(gBuffer[i*width+j].flag==Sampling){
                        continue;
                    };
                    //分别为四个点着色


                    for(int k=0;k<4;k++){
                        if(z[k]==zBuffer[4*(i*width+j)+k]){
                            gBuffer[i*width+j].flag=::Sampling4;
                            gBuffer[i*width+j].index[k]=0;
                            float fixHa= ha[k] * a.z / z[k];
                            float fixHb= hb[k] * b.z / z[k];
                            float fixHc= hc[k] * c.z / z[k];

                            auto u= ua * fixHa + ub * fixHb + uc * fixHc;
                            auto v= va * fixHa + vb * fixHb + vc * fixHc;

                            //插值顶点法向量
                            auto vNormal= (aNormal * fixHa) + (bNormal * fixHb) + (cNormal * fixHc);
                            vNormal.normalization();
                            auto gBufferIndex=gBufferCache->getIndex();

                            gBuffer[i*width+j].index[k]=gBufferIndex;
                            auto gBuffer2=&(gBufferCache->cache[gBufferIndex]);
                            setGBuffer(gBuffer2,object,u,v,vNormal,T,B,N);

                        }
                    }

                }


            }
        }
    }




}

void FragmentShader::shading() {
    if(antiAliasing==render::AntiAliasing::MSAAx4){
        shadingMSAAx4();
    }else{
        shadingNoAnti();
    }

}
