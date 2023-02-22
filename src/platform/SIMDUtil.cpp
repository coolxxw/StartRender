//
// Created by xxw on 2023/2/13.
//

#include <intrin.h>
#include "SIMDUtil.h"

SIMDUtil::Support SIMDUtil::support;

SIMDUtil::Support SIMDUtil::getSimdSupportInfo() {
    Support support2={false};

    unsigned int CPUInfo[4] = {0}, ECX = 0, EDX = 0, XCR0_EAX = 0;
    __cpuid(reinterpret_cast<int *>(CPUInfo), 1);
    ECX = CPUInfo[2];
    EDX = CPUInfo[3];

    if(EDX & 0x00800000)
        support2.MMX= true;
    if(EDX & 0x02000000)
        support2.SSE= true;
    if(EDX & 0x04000000)
        support2.SSE2= true;
    if(ECX & 1)
        support2.SSE3= true;
    if(ECX & 0x00000200)
        support2.SSSE3= true;
    if(ECX & 0x00080000)
        support2.SSE4_1= true;
    if(ECX & 0x00100000)
        support2.SSE4_2= true;
    if(ECX & 0x02000000)
        support2.AES= true;
    if(ECX & 0x10000000)
        support2.AVX= true;
    if(ECX & 0x08000000)
        support2.XSAVE= true;

    XCR0_EAX = _xgetbv(0);
    if(XCR0_EAX & 0x00000002)
        support2.OS_SSE= true;
    if(XCR0_EAX & 0x00000004)
        support2.OS_AVX= true;
    if(XCR0_EAX & 0x00000040)
        support2.OS_AVX512= true;

    return support2;
}
