//
// Created by felix on 14/01/2019.
//

#include <algorithm>
#include "Object.h"

wagl::gl::Object::Object(wagl::gl::Object && other) noexcept : id(other.id){
    other.id = 0;
}

wagl::gl::Object &wagl::gl::Object::operator=(wagl::gl::Object &&other) noexcept {
    if (this != &other) {
        std::swap(id, other.id);
    }
    return *this;
}

wagl::gl::Object::Object() : id(0) {
}
