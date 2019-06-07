//
// Created by felix on 14/01/2019.
//

#ifndef WAGL2_OBJECT_H
#define WAGL2_OBJECT_H

#include <glad/glad.h>

namespace wagl {

    namespace gl {

        class Object {
        public:
            Object();

            Object(const Object &) = delete;

            Object &operator=(const Object &) = delete;

            Object(Object &&) noexcept;

            Object &operator=(Object &&) noexcept;

            GLuint id;

        protected:
            ~Object() = default;
        };

    }

}

#endif //WAGL2_OBJECT_H
