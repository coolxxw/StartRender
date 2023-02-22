//
// Created by xxw on 2023/2/11.
//
#include <fstream>
#include <sstream>
#include "ObjectFile.h"



ObjectFile::ObjectFile() = default;

ObjectFile ObjectFile::LoadFromObjFile(const std::string& filename) {
    ObjectFile obj;
    std::ifstream file;
    file.open(filename,std::ios::in);
    if(!file.is_open()){
        return obj;
    }

    std::string str;

    while(!file.eof()){
        getline(file,str);//获取一行
        std::stringstream ss(str);
        std::string lineHead;
        ss>>lineHead;
        if(lineHead=="v"){
            //顶点
            float a,b,c;
            ss>>a>>b>>c;
            obj.vertex.emplace_back(a,b,c);

        }else if(lineHead=="f"){
            //边
            //顶点
            int a,b,c;
            ss>>a>>b>>c;
            obj.face.push_back(Face{a-1,b-1,c-1});
        }
    }

    file.close();

    //验证
    bool check=true;
    for(auto f:obj.face)
    {
        if(f.a<0 || f.a >=obj.vertex.size() ){
            check=false;
            break;
        }
        if(f.b<0 || f.b >=obj.vertex.size() ){
            check=false;
            break;
        }
        if(f.c<0 || f.c >=obj.vertex.size() ){
            check=false;
            break;
        }
    }

    if(check){
        return obj;
    }
    return {};
}

void ObjectFile::destroy() {
    vertex.clear();
    face.clear();
}


