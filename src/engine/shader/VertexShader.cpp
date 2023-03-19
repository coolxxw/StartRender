//
// Created by xxw on 2023/3/8.
//
#include <intrin.h>
#include "VertexShader.h"
#include "../../engine/platform/SIMDUtil.h"

using namespace render_core;

void VertexShader::shading(const Vector4f* in,Vector4f* out,unsigned int count,const Matrix4f* matrix){
    if (SIMDUtil::avx2){

        //装入矩阵
        auto m = matrix->m;
        __m256 matrix1=  _mm256_set_ps(m[3][0],m[2][0],m[1][0],m[0][0],m[3][0],m[2][0],m[1][0],m[0][0]);
        __m256 matrix2=  _mm256_set_ps(m[3][1],m[2][1],m[1][1],m[0][1],m[3][1],m[2][1],m[1][1],m[0][1]);
        __m256 matrix3= _mm256_set_ps(m[3][2],m[2][2],m[1][2],m[0][2],m[3][2],m[2][2],m[1][2],m[0][2]);
        __m256 matrix4=  _mm256_set_ps(m[3][3],m[2][3],m[1][3],m[0][3],m[3][3],m[2][3],m[1][3],m[0][3]);

        auto len=count/2;
        for (auto i=0;i<len;i+=2){
            //一次转换两个向量
            Vector4f vector = in[2 * i];
            Vector4f vector2 = in[2 * i + 1];
            __m256 vec, v1, v2;
            __m128 vec1, vec2, div1;
            vec1 = _mm_set_ps1(vector.x);
            vec2 = _mm_set_ps1(vector2.x);
            vec = _mm256_set_m128(vec1, vec2);
            v2 = _mm256_mul_ps(matrix1, vec);

            vec1 = _mm_set_ps1(vector.y);
            vec2 = _mm_set_ps1(vector2.y);
            vec = _mm256_set_m128(vec1, vec2);
            v1 = _mm256_mul_ps(matrix2, vec);
            v2 = _mm256_add_ps(v1, v2);

            vec1 = _mm_set_ps1(vector.z);
            vec2 = _mm_set_ps1(vector2.z);
            vec = _mm256_set_m128(vec1, vec2);
            v1 = _mm256_mul_ps(matrix3, vec);
            v2 = _mm256_add_ps(v1, v2);

            vec1 = _mm_set_ps1(vector.w);
            vec2 = _mm_set_ps1(vector2.w);
            vec = _mm256_set_m128(vec1, vec2);
            v1 = _mm256_mul_ps(matrix4, vec);
            v2 = _mm256_add_ps(v1, v2);
            _mm256_storeu2_m128i((__m128i *) &vec1, (__m128i *) &vec2, *(__m256i *) &v2);

            float d[4];
            _mm_store_ps(d, vec1);
            div1 = _mm_set_ps1(d[3]);
            vec1 = _mm_div_ps(vec1, div1);
            _mm_store_ps((float *) &(out[i]), vec1);

            _mm_store_ps(d, vec2);
            div1 = _mm_set_ps1(d[3]);
            vec2 = _mm_div_ps(vec2, div1);
            _mm_store_ps((float *) &(out[i]), vec2);

        }

        if (count % 2) {
            unsigned int i = count - 1;
            Vector4f v = *matrix * in[i];
            out[i] = v * (1 / v.w);
        }

        return;
    }



    for(auto i=0;i<count;i++){
        Vector4f v=*matrix*in[i];
        out[i]=v*(1/v.w);
    }

    return;

}

void
VertexShader::vertexAttributeShading(unsigned int indicesCount, const unsigned int *indices, unsigned int vertexCount,
                                     const Vector4f *vertices,const Vector3f* normal, const float* uv,VertexAttribute *attribute) {

    //顶点法线计算




    if(uv== nullptr){
        for(int i=0;i<vertexCount;i++){
            attribute[i].u=0;
            attribute[i].v=0;
        }
    }else{
        for(int i=0;i<vertexCount;i++){
            attribute[i].u=uv[2*i];
            attribute[i].v=uv[2*i+1];
        }
    }

    if(normal){
        for(int i=0;i<vertexCount;i++){
            attribute[i].normal=normal[i];
        }
    }else{
        for(int i=0;i<vertexCount;i++){
            attribute[i].normal=Vector3f(0,0,0);

        }


        unsigned count=indicesCount/3;
        for ( int i=0;i<count;i++){
            auto ai=indices[3*i];
            auto bi=indices[3*i+1];
            auto ci=indices[3*i+2];

            auto a=vertices[ai].vec3();
            auto b=vertices[bi].vec3();
            auto c=vertices[ci].vec3();

            auto ac=c-a;
            auto ab=b-a;

            //法向量
            auto n=ac.cross(ab);
            //面积
            auto w=1/sqrt(n.x*n.x+n.y*n.y+n.z*n.z);//面积倒数作为权值
            n.normalization();
            //叠加法向量
            attribute[ai].normal=attribute[ai].normal+(n*w);
            attribute[bi].normal=attribute[bi].normal+(n*w);
            attribute[ci].normal=attribute[ci].normal+(n*w);
        }


        for(int i=0;i<vertexCount;i++){

            attribute[i].normal.normalization();

        }

    }


}
