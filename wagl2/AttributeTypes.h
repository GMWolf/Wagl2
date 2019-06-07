//
// Created by felix on 15/01/2019.
//

#ifndef WAGL2_ATTRIBUTETYPES_H
#define WAGL2_ATTRIBUTETYPES_H

#include <glad/glad.h>

namespace wagl {
    template<class T>
    struct attribute_type{
        static const GLint size = 0;
        static const GLenum type = 0;
    };
}

template<>
struct wagl::attribute_type<int> {
    static const GLint size = 1;
    static const GLenum type = GL_INT;
};

template<>
struct wagl::attribute_type<float> {
    static const GLint size = 1;
    static const GLenum type = GL_FLOAT;
};

template<class T, int SIZE>
struct wagl::attribute_type<T[SIZE]> {
    static const GLint size = SIZE;
    static const GLenum type = attribute_type<T>::type;
};
#endif //WAGL2_ATTRIBUTETYPES_H
