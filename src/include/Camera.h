//
// Created by xxw on 2023/2/21.
//

#ifndef STARTRENDER_CAMERA_H
#define STARTRENDER_CAMERA_H

namespace render_core{
    class ContextCache;
}

namespace render{
    class StartRender;
    class Camera {
    private:
        friend class render::StartRender;

        render_core::ContextCache *context;
        explicit Camera(render_core::ContextCache *context): context(context){}

    public:
        void moveForward(double m);
        void  moveLeft(double m);
        void  rotateLeft(double r);
        void  rotateUp(double r);

        //将相机绕世界中心点选择,a为向屏幕右方向旋转弧度,b为向屏幕下方旋转弧度
        void  rotateAroundCenter(double a, double b);

    };
}




#endif //STARTRENDER_CAMERANODE_H
