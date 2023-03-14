//
// Created by xxw on 2023/2/21.
//

#ifndef STARTRENDER_ACCESSOR_H
#define STARTRENDER_ACCESSOR_H

namespace render_core{
    class Context;
}
using render_core::Context;

namespace render{

    class Accessor{
    private:
        Context* context;

    public:
        explicit Accessor(Context* context= nullptr);


    };
}


#endif //STARTRENDER_ACCESSOR_H
