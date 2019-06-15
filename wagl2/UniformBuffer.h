//
// Created by felix on 5/5/2019.
//

#ifndef WAGL2_UNIFORMBUFFER_H
#define WAGL2_UNIFORMBUFFER_H

#include "gl/Buffer.h"

namespace wagl {

    template<class T>
    class UniformBuffer {
    public:

        UniformBuffer();

        T* operator->();

        T& operator*();

        T* get();

        void bind(GLuint index) const;

        void invalidate() {
            buffer.unmap();
            ptr = (T*)buffer.mapRange(0, sizeof(T),
                    GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        }
    private:
        gl::Buffer buffer;
        T* ptr;
    };


    template<class T>
    void UniformBuffer<T>::bind(GLuint index) const {
        buffer.bindRange(GL_UNIFORM_BUFFER, index, 0, sizeof(T));
    }

    template<class T>
    T *UniformBuffer<T>::get() {
        return ptr;
    }

    template<class T>
    T &UniformBuffer<T>::operator*() {
        return *ptr;
    }

    template<class T>
    T *UniformBuffer<T>::operator->() {
        return ptr;
    }

    template<class T>
    UniformBuffer<T>::UniformBuffer()
    : buffer(sizeof(T), nullptr, GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT) {
        ptr = (T*)buffer.mapRange(0, sizeof(T),
                GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT);
    }


}

#endif //WAGL2_UNIFORMBUFFER_H
