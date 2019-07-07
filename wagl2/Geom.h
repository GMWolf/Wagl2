//
// Created by felix on 04/06/2019.
//

#ifndef WAGL2_GEOM_H
#define WAGL2_GEOM_H

#include <glm/glm.hpp>

namespace wagl {

    template<typename T>
    struct rect_t {
        glm::vec<2, T> min;
        glm::vec<2, T> max;

        glm::vec<2, T> size() const {
            return max - min;
        }

        template<typename E>
        bool contains(glm::vec<2, E> p) const {
            return glm::all(glm::lessThanEqual(min, p) && glm::greaterThan(max, p));
        }


        rect_t<T> operator+(glm::vec<2, T> p) const {
            return {
                min + p,
                max + p
            };
        };
        rect_t<T> operator-(glm::vec<2, T> p) const {
            return {
                min - p,
                max - p
            };
        }

        rect_t<T> operator-(T p) const {
            return {
                    min - p,
                    max - p
            };
        }

        rect_t<T> operator*(glm::vec<2, T> p) const {
            return {
                min * p,
                max * p
            };
        }

        rect_t<T> operator*(T t) const {
            return {
                min * t,
                max * t
            };
        }

    };

    using rect = rect_t<float>;
    using irect = rect_t<int>;
    using urect = rect_t<unsigned int>;
}

#endif //WAGL2_GEOM_H
