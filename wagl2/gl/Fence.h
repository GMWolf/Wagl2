//
// Created by felix on 5/11/2019.
//

#ifndef WAGL2_FENCE_H
#define WAGL2_FENCE_H

#include <glad/glad.h>

namespace wagl::gl {

    class Fence {
        GLsync obj = nullptr;

    public:

        ~Fence();

        void sync();

        void wait();
    };

}

#endif //WAGL2_FENCE_H
