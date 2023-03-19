//
// Created by xxw on 2023/3/18.
//

#ifndef STARTRENDER_IMAGETOOL_H
#define STARTRENDER_IMAGETOOL_H


#include <string>

namespace render{
    class ImageTool {
    public:
        //解析image 返回RGBA格式 使用delete 释放返回指针
        //image必须是png jpeg格式
        static void* imageToRGBA(std::string file,unsigned int *width,unsigned int *height);

        //解析image 返回RGBA格式 使用delete 释放返回指针
        //image必须是png jpeg格式
        //data必须是image原始数据
        static void* imageToRGBA(const void* data,unsigned int dataLength,unsigned int *width,unsigned int *height);

        static void* PngToRGBA(const void* data,unsigned int dataLength,unsigned int *width,unsigned int *height);

        static void* JpegToRGBA(const void* data,unsigned int dataLength,unsigned int *width,unsigned int *height);

    };
}




#endif //STARTRENDER_IMAGETOOL_H
