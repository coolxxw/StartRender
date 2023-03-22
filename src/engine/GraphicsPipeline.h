//
// Created by xxw on 2023/3/8.
//

#ifndef STARTRENDER_GRAPHICSPIPELINE_H
#define STARTRENDER_GRAPHICSPIPELINE_H

#include "data/Context.h"
#include "shader/RasterizationShader.h"
#include "data/GBuffer.h"

namespace render_core{

    class GraphicsPipeline {
    private:
        Context *context;

        void prepare();

    public:

        explicit GraphicsPipeline(Context *context): context(context){};
        ~GraphicsPipeline();
        void render();
    };
}



#endif //STARTRENDER_GRAPHICSPIPELINE_H
