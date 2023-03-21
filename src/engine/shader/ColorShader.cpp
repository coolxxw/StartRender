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
            if(gBuffer[i*width+j].flag==GBufferFlag::Sampling){
                frame[i*width+j]=RGBA(255,255,255,255);
                continue;
            }

            auto f= gBuffer[i*width+j].normal;
            RGBAF color=RGBAF(f.x,f.y,f.z);
            frame[i*width+j]=RGBA(color);
        }
    }
}

void
ColorShader::shadingBaseColor() const {
    RGBA* frame=(RGBA*)framebuffer;
    for(int i=0;i<height;i++){
        for (int j=0;j<width;j++){
            auto color= gBuffer[i*width+j].baseColor;
            frame[i*width+j]=RGBA(color);
        }
    }
}

void ColorShader::shadingMetal() const {
    RGBA* frame=(RGBA*)framebuffer;
    for(int i=0;i<height;i++){
        for (int j=0;j<width;j++){
            auto f= gBuffer[i*width+j].metallic;
            auto color=RGBAF(f,f,f);
            frame[i * width + j] =  RGBA(color);
        }
    }
}

void
ColorShader::shadingRoughness() const {
    RGBA *frame = (RGBA *) framebuffer;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            auto f = gBuffer[i * width + j].roughness;
            auto color=RGBAF(f,f,f);
            frame[i * width + j] =  RGBA(color);
        }
    }
}

void
ColorShader::shadingEmission() const {
    RGBA *frame = (RGBA *) framebuffer;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            auto color = gBuffer[i * width + j].emission;
            frame[i * width + j] = RGBA(color);
        }
    }
}


static inline RGBAF RgbaMulRgba(RGBAF c1,RGBAF c2){
    return RGBAF(c1.r*c2.r,c1.g*c2.g,c1.b*c2.b,c1.a*c2.a);
}

static inline RGBAF RgbMulFloat(RGBAF c1,float f){
    return RGBAF(c1.r*f,c1.g*f,c1.b*f,c1.a);
}

static inline RGBAF RgbAdd(RGBAF c1,RGBAF c2){
    return RGBAF(c1.r+c2.r,c1.g+c2.g,c1.b+c2.b,c1.a);
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

static RGBA inline pbr(const Environment& env, Vector3f n,Vector3f v,Vector3f l,RGBAF color,RGBAF emission,float metallic,float perceptualRoughness,const CubeMap *skybox){

    RGBAF finalColor=RGBAF(0,0,0);//最终颜色

    //计算specular_brdf
    Vector3f h=v+l;
    h.normalization();


    //roughness平方 转换成真实粗糙度
    float roughness= perceptualRoughness*perceptualRoughness;
    roughness=roughness<0.02f?0.02f:roughness;

    float diffuseRate=1;//漫反射率
    Vector3f specF0=Vector3f{0,0,0};//镜面反射f0
    //转换漫反射 f0
    DiffuseAndSpecularFromMetallic(color,metallic,&specF0,&diffuseRate);

    RGBAF diffuseColor=RGBAF(0,0,0);
    RGBAF specularColor=RGBAF(0,0,0);

    RGBAF ambientSpecularLight;

    float NdotL=n*l;
    float NdotV=n*v;

    //直射光
    if(NdotL>0 && NdotV>0){

        //计算漫反射颜色
        float diffuseTerm= DisneyDiffuse(NdotV,NdotL,l*h,perceptualRoughness);
        auto diffuseColorTerm=diffuseTerm*NdotL*diffuseRate;
        diffuseColor=RgbMulFloat(color,diffuseColorTerm);
        diffuseColor= RgbaMulRgba(env.directLightDiffuseFact,diffuseColor);

        //计算镜面反射
        float specularTerm= GGXTerm(n*h,roughness)* GGXVisibilityTerm(NdotL,NdotV,roughness);
        //Gamma
        specularTerm= sqrt(specularTerm);
        specularTerm=specularTerm<1e-4f?1e-4f:specularTerm;
        specularTerm*=NdotL;

        //计算菲涅尔效应
        Vector3f fresnelTerm= FresnelTerm(specF0*(1-diffuseColorTerm),h*l);
        auto specularColorTerm= fresnelTerm*specularTerm;
        specularColor.r=env.directLightSpecularFact.r*specularColorTerm.x;
        specularColor.g=env.directLightSpecularFact.g*specularColorTerm.y;
        specularColor.b=env.directLightSpecularFact.b*specularColorTerm.z;
    }


    //非直射光漫反射
    RGBAF indirectDiffuseColor= RgbMulFloat( color,diffuseRate);
    indirectDiffuseColor= RgbaMulRgba( indirectDiffuseColor,env.indirectLightDiffuseFact);

    //计算非直射光镜面反射
    RGBAF indirectSpecularColor=RGBAF(0,0,0);
    if(NdotV>0){
        //计算环境光镜面反射
        float surfaceReduction = 1.0f-0.28f*roughness*perceptualRoughness;
        //surfaceReduction = 1.0 / (roughness*roughness + 1.0);
        Vector3f invView= symmetryVector(v,n);
        ambientSpecularLight = skybox->get(invView);
        float grazingTerm = 1-roughness + (1-diffuseRate);
        auto ambientSpecularColor=FresnelLerp(specF0,grazingTerm,n*v);
        ambientSpecularColor.x*=ambientSpecularLight.r;
        ambientSpecularColor.y*=ambientSpecularLight.g;
        ambientSpecularColor.z*=ambientSpecularLight.b;
        indirectSpecularColor.r=ambientSpecularColor.x*env.indirectLightSpecularFact.r;
        indirectSpecularColor.g=ambientSpecularColor.y*env.indirectLightSpecularFact.g;
        indirectSpecularColor.b=ambientSpecularColor.z*env.indirectLightSpecularFact.b;
    }

    //计算自发光
    auto emissionColor= RgbaMulRgba(emission,env.emissionLightFact);

    auto dColor= RgbAdd(diffuseColor,indirectDiffuseColor);
    auto sColor= RgbAdd(specularColor,indirectSpecularColor);
    finalColor= RgbAdd(dColor,sColor);
    finalColor= RgbAdd(finalColor,emissionColor);

    finalColor.r=finalColor.r>1.0f?1.0f:finalColor.r;
    finalColor.g=finalColor.g>1.0f?1.0f:finalColor.g;
    finalColor.b=finalColor.b>1.0f?1.0f:finalColor.b;

    return RGBA(finalColor);

}


void ColorShader::shadingPbr() const {
    auto l=-environment.directLightVector;
    auto v=-cameraGear;

    //转换Evn
    Environment env=this->environment;
    env.directLightDiffuseFact= RgbaMulRgba(env.directLightDiffuseFact,env.directLightColor);
    env.directLightSpecularFact= RgbaMulRgba(env.directLightSpecularFact,env.directLightColor);
    env.directLightDiffuseFact= RgbMulFloat(env.directLightDiffuseFact,env.directLightDiffuseFact.a);
    env.directLightSpecularFact= RgbMulFloat(env.directLightSpecularFact,env.directLightSpecularFact.a);

    env.indirectLightDiffuseFact= RgbMulFloat(env.indirectLightDiffuseFact,env.indirectLightDiffuseFact.a);
    env.indirectLightSpecularFact= RgbMulFloat(env.indirectLightSpecularFact,env.indirectLightSpecularFact.a);

    env.emissionLightFact= RgbMulFloat(env.emissionLightFact,env.emissionLightFact.a);


    RGBA* frame=(RGBA*)framebuffer;
    for(int i=0;i<height;i++){
        for (int j=0;j<width;j++){
            if(gBuffer[i*width+j].flag==GBufferFlag::Sampling){
                frame[i*width+j]= pbr(env,gBuffer[i*width+j].normal, v, l, gBuffer[i*width+j].baseColor,gBuffer[i*width+j].emission, gBuffer[i*width+j].metallic, gBuffer[i * width + j].roughness, skybox);
            }else if(gBuffer[i*width+j].flag==GBufferFlag::Sampling4){
                auto index= gBuffer[i*width+j].index;
                RGBA color[4];
                int r=0,g=0,b=0;
                for(int k=0;k<4;k++){
                    if(index[k]==0){
                        color[k]=RGBA(0,0,0,255);
                        continue;
                    }
                    auto G=gBufferCache->get(index[k]);
                    color[k]=pbr(env, G->normal, v, l, G->baseColor, G->emission, G->metallic, G->roughness, skybox);
                    r+=(int)color[k].r;
                    g+=(int)color[k].g;
                    b+=(int)color[k].b;
                }
                frame[i*width+j]=RGBA(r/4,g/4,b/4,255);
            }


        }
    }
}


void ColorShader::shading() const{

    memset(framebuffer,127,width*height*4);

    shadingPbr();
}

void ColorShader::shadingZBuffer(float *zBuffer, unsigned int sampling) {
    RGBA* frame=(RGBA*)framebuffer;
    if(sampling==4){
        for(int i=0;i<height  ;i++){
            for (int j=0;j<width ;j++){
                float z=0;
                for(int k=0;k<4;k++){
                    z+=zBuffer[4*(i*width+j)+k];
                }
                z/=4;

                frame[i*width+j]=RGBA(RGBAF(z,z,z));
            }
        }
    }else{
        for(int i=0;i<height  ;i++){
            for (int j=0;j<width ;j++){
                float z=zBuffer[i*width+j];
                frame[i*width+j]=RGBA(RGBAF(z,z,z));
            }
        }
    }


}
