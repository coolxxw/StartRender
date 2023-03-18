//
// Created by xxw on 2023/2/13.
//

#ifndef STARTRENDER_SIMD_H
#define STARTRENDER_SIMD_H

#include "../platform/SIMDUtil.h"
#include <intrin.h>
#include <immintrin.h>

int simdTest(){
    SIMDUtil::Support simdSupport=SIMDUtil::getSimdSupportInfo();

    volatile double num[4]={1.0,2.0,3.0,4.0};
    volatile double num2[4]={5.0,6.0,7.0,8.0};
    __m256d a;
    a= _mm256_load_pd(const_cast<const double *>(num));
    __m256d b= _mm256_load_pd(const_cast<const double *>(num2));
    __m256d c= _mm256_add_pd(a,b);
    _mm256_store_pd(const_cast<double *>(num), c);
    for(int i=0;i<4;i++){
        std::cout<<num[i]<<std::endl;
    }


    return 0;
}


SIMDUtil::Support SIMDUtil::getSimdSupportInfo() {
    Support support={false};

    unsigned int CPUInfo[4] = {0}, ECX = 0, EDX = 0, XCR0_EAX = 0;
    __cpuid(reinterpret_cast<int *>(CPUInfo), 1);
    ECX = CPUInfo[2];
    EDX = CPUInfo[3];

    if(EDX & 0x00800000)
        support.MMX= true;
    if(EDX & 0x02000000)
        support.SSE= true;
    if(EDX & 0x04000000)
        support.SSE2= true;
    if(ECX & 1)
        support.SSE3= true;
    if(ECX & 0x00000200)
        support.SSSE3= true;
    if(ECX & 0x00080000)
        support.SSE4_1= true;
    if(ECX & 0x00100000)
        support.SSE4_2= true;
    if(ECX & 0x02000000)
        support.AES= true;
    if(ECX & 0x10000000)
        support.AVX= true;
    if(ECX & 0x08000000)
        support.XSAVE= true;

    XCR0_EAX = _xgetbv(0);
    if(XCR0_EAX & 0x00000002)
        support.OS_SSE= true;
    if(XCR0_EAX & 0x00000004)
        support.OS_AVX= true;
    if(XCR0_EAX & 0x00000040)
        support.OS_AVX512= true;

    return support;
}



#endif //STARTRENDER_SIMD_H
