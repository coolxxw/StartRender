//
// Created by xxw on 2023/2/27.
//

#include "lodepng_util.h"
#include "pngtest.h"
void png_test(){
    std::vector<unsigned char> png_file_buffer;

    lodepng::load_file(png_file_buffer,"bee.png");
    std::vector<unsigned char> png_decode;
    unsigned int h;
    unsigned int w;
    lodepng::State state;
    lodepng::decode(png_decode,w,h,state,png_file_buffer);
    unsigned char *srgb=new unsigned char[w*h*4];
    lodepng::convertToSrgb(srgb,png_decode.data(),w,h,&state);

    return;
}