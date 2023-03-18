//
// Created by xxw on 2023/3/11.
//
#include <assert.h>
#include <memory>
#include "ColorShader.h"


using namespace render_core;


//输出skybox
void ColorShader::shadingSkyBox() const {
    RGBA* frame=(RGBA*)framebuffer;
    int w=0;
    //计算长宽比
    if(((float )height)/(float)width>0.75){
        w=(int) floor(((float)width)*0.25);
    }else{
        w=(int) floor((float)height/3);
    }
    auto wf=(float)w;

    //top
    for(int i=0;i<w;i++){
        for(int j=w;j<2*w;j++){
            float u=((float)j-wf+0.5f)/wf;
            float v=(((float )i+0.5f)/wf);
            auto color=skybox->top.getAttribute(u,v);
            frame[i*width+j]=RGBA(color);
        }
    }
    //left
    for(int i=w;i<2*w;i++){
        for(int j=0;j<w;j++){
            float u=((float)j+0.5f)/wf;
            float v=(((float )i-wf+0.5f)/wf);
            auto color=skybox->left.getAttribute(u,v);
            frame[i*width+j]=RGBA(color);
        }
    }
    //front
    for(int i=w;i<2*w;i++){
        for(int j=w;j<2*w;j++){
            float u=((float)j-wf+0.5f)/wf;
            float v=(((float )i-wf+0.5f)/wf);
            auto color=skybox->front.getAttribute(u,v);
            frame[i*width+j]=RGBA(color);
        }
    }
    //right
    for(int i=w;i<2*w;i++){
        for(int j=2*w;j<3*w;j++){
            float u=((float)j-2*wf+0.5f)/wf;
            float v=(((float )i-wf+0.5f)/wf);
            auto color=skybox->right.getAttribute(u,v);
            frame[i*width+j]=RGBA(color);
        }
    }
    //back
    for(int i=w;i<2*w;i++){
        for(int j=3*w;j<4*w;j++){
            float u=((float)j-3*wf+0.5f)/wf;
            float v=(((float )i-wf+0.5f)/wf);
            auto color=skybox->back.getAttribute(u,v);
            frame[i*width+j]=RGBA(color);
        }
    }
    //bottom
    for(int i=2*w;i<3*w;i++){
        for(int j=w;j<2*w;j++){
            float u=((float)j-wf+0.5f)/wf;
            float v=(((float )i-3*wf+0.5f)/wf);
            auto color=skybox->bottom.getAttribute(u,v);
            frame[i*width+j]=RGBA(color);
        }
    }



}




void ColorShader::shadingTexture(unsigned int width, unsigned int height, TextureMap texture, void *framebuffer) {
    RGBA* frame=(RGBA*)framebuffer;
    for(int i=0;i<height/2 && i<texture.height;i++){
        for (int j=0;j<width/2 && j<texture.width;j++){
            frame[i*width+j]=texture.texture[i*texture.width+j];
        }
    }
}





void ColorShader::shadingNormal() const {
    RGBA* frame=(RGBA*)framebuffer;
    for(int i=0;i<height;i++){
        for (int j=0;j<width;j++){
            auto f= gBuffer[i*width+j].normal;
            RGBA color;
            color.r=(int)((float)255*(f.x*0.5+0.5));
            color.g=(int)((float)255*(f.y*0.5+0.5));
            color.b=(int)((float)255*(f.z*0.5+0.5));
            color.a=(int)((float)255*1);

            frame[i*width+j]=color;
        }
    }
}

void
ColorShader::shadingBaseColor() const {
    RGBA* frame=(RGBA*)framebuffer;
    for(int i=0;i<height;i++){
        for (int j=0;j<width;j++){
            auto f= gBuffer[i*width+j].baseColor;
            RGBA color;
            color.r=(int)((float)255*f.r);
            color.g=(int)((float)255*f.g);
            color.b=(int)((float)255*f.b);
            color.a=(int)((float)255*f.a);

            frame[i*width+j]=color;
        }
    }
}

void ColorShader::shadingMetal() const {
    RGBA* frame=(RGBA*)framebuffer;
    for(int i=0;i<height;i++){
        for (int j=0;j<width;j++){
            auto f= gBuffer[i*width+j].metal;
            RGBA color;
            color.r=(int)((float)255*f);
            color.g=(int)((float)255*f);
            color.b=(int)((float)255*f);
            color.r=(int)((float)255*f);

            frame[i*width+j]=color;
        }
    }
}

void
ColorShader::shadingRoughness() const {
    RGBA *frame = (RGBA *) framebuffer;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            auto f = gBuffer[i * width + j].roughness;
            RGBA color;
            color.r = (int) ((float) 255 * f);
            color.g = (int) ((float) 255 * f);
            color.b = (int) ((float) 255 * f);
            color.a = (int) ((float) 255 * f);

            frame[i * width + j] = color;
        }
    }
}


//法线分布函数
//n法向量 h半程向量
//alpha=Roughness^2
//省去分母pi
static inline float GGXTerm(float NdotH, float alpha) {
    float alpha2 = alpha * alpha;
    float d = NdotH *(alpha2-NdotH)* NdotH + 1.0f;
    return alpha2  / (d * d + 1e-7f);

}

//计算可见项 包括几何函数和配平系数
static inline float GGXVisibilityTerm(float NdotL,float NdotV,float roughness){
    float a=roughness;
    float lambdaV=NdotL*(NdotV*(1-a)+a);
    float lambdaL=NdotV*(NdotL*(1-a)+a);
    return 0.5f/(lambdaV+lambdaL+1e-5f);
}


static inline Vector3f FresnelTerm(Vector3f f0,float HdotV){
    float t= pow(1-HdotV,5.0f);
    return Vector3f{f0.x+(1-f0.x)*t,f0.y+(1-f0.y)*t,f0.z+(1-f0.z)*t};
}

static inline float FresnelTerm(float f0,float HdotV){
    float t= pow(1-HdotV,5.0f);
    return f0+(1-f0)*t;
}

inline Vector3f FresnelLerp (Vector3f f0, float f90, float cosA)
{
    float t = pow (1 - cosA,5.0f);   // ala Schlick interpoliation
    float f=f90*t;
    return f0+Vector3f(f,f,f);
}


static inline float DisneyDiffuse(float NdotV,float NdotL,float LdotH,float perceptualRoughness){
    float fd90=0.5f+2*LdotH*LdotH*perceptualRoughness;
    float lightScatter=(1.0f+(fd90-1)* pow(1-NdotL,5.0f));//入射反向菲涅尔
    float viewScatter=(1.0f+(fd90-1)* pow(1-NdotV,5.0f));//出射反向菲涅尔
    assert(lightScatter>=0);
    assert(viewScatter>=0);
    return lightScatter*viewScatter;
}

static inline void DiffuseAndSpecularFromMetallic(RGBAF baseColor,float metallic,Vector3f* specColor,float*oneMinusReflectivity  ){
    //高光反射率
    specColor->x=baseColor.r*metallic+0.04f*(1.0f-metallic);
    specColor->y=baseColor.g*metallic+0.04f*(1.0f-metallic);
    specColor->z=baseColor.b*metallic+0.04f*(1.0f-metallic);
    //漫反射系数
    *oneMinusReflectivity=0.96f*(1.0f-metallic);
}

//求a关于b的对称向量
//a b均为法向量
static inline Vector3f symmetryVector(const Vector3f& a,const Vector3f& b){
    float x2=b.x*b.x;
    float y2=b.y*b.y;
    float z2=b.z*b.z;
    float xx=a.x*b.x;
    float yy=a.y*b.y;
    float zz=a.z*b.z;

    float x=(x2-y2-z2)*a.x+2*b.x*(yy+zz);
    float y=(y2-x2-z2)*a.y+2*b.y*(xx+zz);
    float z=(z2-y2-x2)*a.z+2*b.z*(xx+yy);
    return Vector3f(x,y,z);
}

static RGBA inline pbr(Vector3f n,Vector3f v,Vector3f l,RGBAF color,float metallic,float perceptualRoughness,const CubeMap *skybox){
    //计算specular_brdf
    Vector3f h=v+l;
    h.normalization();

    float ambient=0.3;//环境光
    float light=0.6;//直射光

    //roughness平方 转换成真实粗糙度
    float roughness= perceptualRoughness*perceptualRoughness;
    roughness=roughness<0.02f?0.02f:roughness;

    float diffuseRate=1;//漫反射率
    Vector3f specColor=Vector3f{0,0,0};//镜面反射f0
    //转换漫反射 f0
    DiffuseAndSpecularFromMetallic(color,metallic,&specColor,&diffuseRate);

    float diffuseColor=0;
    Vector3f specularColor=Vector3f{0,0,0};

    RGBAF ambientSpecularLight;

    float NdotL=n*l;
    float NdotV=n*v;

    if(NdotL>0 && NdotV>0){

        //计算漫反射
        float diffuseTerm= DisneyDiffuse(NdotV,NdotL,l*h,perceptualRoughness);
        diffuseColor=diffuseTerm*NdotL*diffuseRate;


        //计算镜面反射
        float specularTerm= GGXTerm(n*h,roughness)* GGXVisibilityTerm(NdotL,NdotV,roughness);
        //Gamma
        specularTerm= sqrt(specularTerm);
        specularTerm=specularTerm<1e-4f?1e-4f:specularTerm;
        specularTerm*=NdotL;

        //计算菲涅尔效应
        Vector3f fresnelTerm= FresnelTerm(specColor*(1-diffuseColor),h*l);

        specularColor=fresnelTerm*(specularTerm);


    }


    //补上环境光
    diffuseColor=diffuseColor*light+ambient*diffuseRate;




    //计算环境光反射
    if(NdotV>0){
        //计算环境光镜面反射
        float surfaceReduction = 1.0f-0.28f*roughness*perceptualRoughness;
        //surfaceReduction = 1.0 / (roughness*roughness + 1.0);
        Vector3f invView= symmetryVector(v,n);
        ambientSpecularLight = skybox->get(invView);
        float grazingTerm = 1-roughness + (1-diffuseRate);
        auto ambientSpecularColor=FresnelLerp(specColor,grazingTerm,n*v);
        ambientSpecularColor.x*=ambientSpecularLight.r;
        ambientSpecularColor.y*=ambientSpecularLight.g;
        ambientSpecularColor.z*=ambientSpecularLight.b;
        specularColor=specularColor*light+ambientSpecularColor;
    }



    float r=(color.r*(diffuseColor))+specularColor.x;
    float g=(color.g*(diffuseColor))+specularColor.y;
    float b=(color.b*(diffuseColor))+specularColor.z;

    r=r>1?1:r;
    g=g>1?1:g;
    b=b>1?1:b;

    return RGBA(RGBAF(r,g,b));

}


void ColorShader::shadingPbr() const {
    auto l=-lightParam->directionalLight;
    auto v=-cameraGear;

    RGBA* frame=(RGBA*)framebuffer;
    for(int i=0;i<height;i++){
        for (int j=0;j<width;j++){
            if(gBuffer[i*width+j].valid){
                frame[i*width+j]= pbr(gBuffer[i*width+j].normal,v,l,gBuffer[i*width+j].baseColor,gBuffer[i*width+j].metal,gBuffer[i*width+j].roughness,skybox);
            }


        }
    }
}


void ColorShader::shading() const{

    memset(framebuffer,127,width*height*4);

    shadingPbr();
}
