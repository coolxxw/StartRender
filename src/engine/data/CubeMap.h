//
// Created by xxw on 2023/3/18.
//

#ifndef STARTRENDER_CUBEMAP_H
#define STARTRENDER_CUBEMAP_H



#include "../../include/type.h"
#include "../../math/Vector.h"
#include "TextureMap.h"

namespace render_core {
    struct CubeMap{
    public:
        TextureMap top;
        TextureMap left;
        TextureMap right;
        TextureMap bottom;
        TextureMap front;
        TextureMap back;

        CubeMap(){
            const static RGBA defaultColor=RGBA();
            const static RGBA defaultTexture[2*2]={defaultColor, defaultColor, defaultColor, defaultColor};
            static TextureMap defaultTextureMap;
            defaultTextureMap.texture=defaultTexture;
            defaultTextureMap.height=1;
            defaultTextureMap.width=1;
            top=defaultTextureMap;
            left=defaultTextureMap;
            right=defaultTextureMap;
            bottom=defaultTextureMap;
            front=defaultTextureMap;
            back=defaultTextureMap;
        }


        RGBAF get(const Vector3f& vec) const{
            TextureMap texture=top;
            //相对贴图中心点坐标
            float cx;
            float cy;

            float x2=vec.x*vec.x;
            float y2=vec.y*vec.y;
            float z2=vec.z*vec.z;


            if(x2>y2){
                if(x2>z2){
                    if(vec.x>0){
                        //left
                        texture=left;
                        cx=vec.z/vec.x;
                        cy=vec.y/vec.x;
                    }else{
                        //right
                        texture=right;
                        cx=vec.z/vec.x;
                        cy=-vec.y/vec.x;
                    }
                }else{
                    if(vec.z>0){
                        //front
                        texture=front;
                        cx=-vec.x/vec.z;
                        cy=vec.y/vec.z;
                    }else{
                        //back;
                        texture=back;
                        cx=-vec.x/vec.z;
                        cy=-vec.y/vec.z;
                    }
                }
            }else{
                if(y2>z2){
                    if(vec.y>0){
                        //top
                        texture=top;
                        cx=-vec.x/vec.y;
                        cy=-vec.z/vec.y;
                    }else{
                        //bottom
                        texture=bottom;
                        cx=-vec.x/vec.y;
                        cy=-vec.z/vec.y;
                    }
                }else{
                    if(vec.z>0){
                        //front
                        texture=front;
                        cx=-vec.x/vec.z;
                        cy=vec.y/vec.z;
                    }else{
                        //back;
                        texture=back;
                        cx=-vec.x/vec.z;
                        cy=-vec.y/vec.z;
                    }
                }
            }

            cx=cx*0.5f+0.5f;
            cy=0.5f-cy*0.5f;
            return texture.getAttribute(cx,cy);
        }
    };

}



#endif //STARTRENDER_CUBEMAP_H

