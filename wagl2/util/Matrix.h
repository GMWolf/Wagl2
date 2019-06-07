//
// Created by felix on 06/06/2019.
//

#ifndef WAGL2_MATRIX_H
#define WAGL2_MATRIX_H
#include <vector>

namespace wagl {

    template<class T>
    class Matrix {
        std::vector <T> store;

    public:

        using iterator = typename std::vector<T>::iterator;
        using const_iterator = typename std::vector<T>::const_iterator;

        class Column {
            iterator _begin;
            iterator _end;

            Column(iterator _begin, iterator _end) : _begin(_begin), _end(_end) {}

            friend class Matrix<T>;

        public:
            T &operator[](size_t y) {
                return *(_begin + y);
            }

            const T &operator[](size_t y) const {
                return *(_begin + y);
            }

            iterator begin() {
                return _begin;
            }

            const_iterator begin() const {
                return _begin;
            }

            iterator end() {
                return _end;
            }

            const_iterator end() const {
                return _end;
            }
        };


        size_t width, height;

        Matrix(size_t w, size_t h) :
                width(w), height(h) {
            store.resize(w * h);
        }

        Matrix() : width(0), height(0) {
        }

        void realloc(size_t w, size_t h) {
            store.resize(w * h);
            width = w;
            height = h;
        }

        T &operator()(size_t x, size_t y) {
            assert(x < width);
            assert(y < height);
            return store[x * height + y];
        }

        const T &operator()(size_t x, size_t y) const {
            assert(x < width);
            assert(y < height);
            return store[x * height + y];
        }

        iterator begin() {
            return store.begin();
        }

        const_iterator begin() const {
            return store.begin();
        }

        iterator end() {
            return store.end();
        }

        const iterator end() const {
            return store.end();
        }

        const Column operator[](size_t x) const {
            return Column(store.begin() + (x * height),
                          store.begin() + ((x + 1) * height));
        }

        Column operator[](size_t x) {
            return Column(store.begin() + (x * height),
                          store.begin() + ((x + 1) * height));
        }

    };
}
#endif //WAGL2_MATRIX_H
