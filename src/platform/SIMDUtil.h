//
// Created by xxw on 2023/2/13.
//

#ifndef STARTRENDER_SIMDUTIL_H
#define STARTRENDER_SIMDUTIL_H


class SIMDUtil {
public:
    struct Support{
        bool MMX;
        bool SSE;
        bool SSE2;
        bool SSE3;
        bool SSSE3;
        bool SSE4_1;
        bool SSE4_2;
        bool AES;
        bool AVX;
        bool XSAVE;
        bool OS_SSE;
        bool OS_AVX;
        bool OS_AVX512;
    };

    static Support support;

    static Support getSimdSupportInfo();

};


#endif //STARTRENDER_SIMDUTIL_H
