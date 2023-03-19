//
// Created by xxw on 2023/3/18.
//

#include <string>
#include <fstream>
#include <vector>
#include "ImageTool.h"
#include "lodepng.h"
#include "turbojpeg.h"


void *render::ImageTool::imageToRGBA(std::string file, unsigned int *width, unsigned int *height) {
    std::ifstream f;
    f.open(file,std::ios::in|std::ios::binary);
    if(f.is_open()) {
        f.seekg(0, std::ios::end);
        auto len = f.tellg();
        f.seekg(0, std::ios::beg);
        auto data = new char[len];
        f.read((char *) data, len);
        f.close();

        auto d=imageToRGBA(data,len,width,height);
        delete[] data;

        return d;
    }
    return nullptr;
}

void *
render::ImageTool::imageToRGBA(const void *data, unsigned int dataLength, unsigned int *width, unsigned int *height) {
    auto* imageData=(unsigned char*)data;
    if (dataLength>=4&&imageData[0]==0x89 && imageData[1]==0x50&&imageData[2]==0x4E&&imageData[3]==0x47){
        return ImageTool::PngToRGBA(imageData,dataLength,width,height);
    }
    if (dataLength>=3&&imageData[0]==0xFF && imageData[1]==0xD8&&imageData[2]==0xFF){
        return ImageTool::JpegToRGBA(imageData,dataLength,width,height);
    }
    return nullptr;
}

void *render::ImageTool::PngToRGBA(const void *data, unsigned int dataLength, unsigned int *width, unsigned int *height) {
    std::vector<unsigned char > png ;
    lodepng::decode(png,*width,*height,(unsigned char*)data,dataLength);
    if(*width>0 && *height>0){
        auto* buffer=new unsigned char [(*width)*(*height)*4];
        memmove(buffer,png.data(),*width*(*height)*4);
        return buffer;
    }

    return nullptr;
}

void *render::ImageTool::JpegToRGBA(const void *data, unsigned int dataLength, unsigned int *width, unsigned int *height) {
    //调用libjpeg turbo
    auto tjHandle=tjInitDecompress();
    if(tjHandle==nullptr){
        return nullptr;
    }
    long unsigned int jpegSize=dataLength; //!< _jpegSize from above
    auto* compressedImage=new unsigned char [dataLength]; //!< _compressedImage from above
    memmove(compressedImage,data,dataLength);

    int jpegSubsamp, jWidth, jHeight;
    tjDecompressHeader(tjHandle,compressedImage,jpegSize,&jWidth,&jHeight);

    if(jHeight*jHeight>0){
        int bufferSize=jWidth*jHeight*4;
        auto* buffer=new unsigned char [jWidth*jHeight*4];
        tjDecompress(tjHandle,compressedImage,jpegSize,buffer,jWidth,0,jHeight,4,TJFLAG_FASTDCT);
        delete[] compressedImage;
        tjDestroy(tjHandle);

        *width=jWidth;
        *height=jHeight;

        return buffer;
    }
    return nullptr;

}
