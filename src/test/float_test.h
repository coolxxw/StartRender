//
// Created by xxw on 2023/2/13.
//

#ifndef STARTRENDER_FLOAT_TEST_H
#define STARTRENDER_FLOAT_TEST_H


#include <iostream>
#include <ctime>

float floatAdd(){

    srand(123456);
    volatile float a=rand()+rand()-(float )rand()/rand();
    volatile float b=0;
    for(int i=0;i<100000000;i++){
        b=b*a;
        b=b/a;
    }
    return b;
}

double doubleAdd(){

    srand(123456);
    volatile double a=rand()+rand()-(double )rand()/rand();
    volatile double b=0;
    for(int i=0;i<100000000;i++){
        b=b*a;
        b=b/a;
    }
    return b;
}

int floatTest(){
    system("chcp 65001");
    std::cout<<"浮点数性能测试"<<std::endl;
    int t=clock();
    volatile float b= floatAdd();
    int t2=clock();
    std::cout<<(t2-t)<<std::endl;
    t=t2;
    b=doubleAdd();
    t2=clock();
    std::cout<<(t2-t)<<std::endl;
    return 0;
}






#endif //STARTRENDER_FLOAT_TEST_H
