//
// Created by felix on 5/11/2019.
//

#ifndef WAGL2_UTILS_H
#define WAGL2_UTILS_H

#include <vector>
#include <memory>

namespace wagl::util {

    struct block {
        size_t size;
        struct deleter {
            void operator()(char* p) {
                free(p);
            }
        };
        std::unique_ptr<char, deleter> data;

        explicit block(size_t size) : size(size) {
            data = std::unique_ptr<char, deleter>((char*)malloc(size));
        }

        block(const block&) = delete;
        block(block&&) noexcept = default;
        block& operator=(const block&) = delete;
        block& operator=(block&&) noexcept  = default;

        char* operator->() {
            return data.get();
        }

        char* get() {
            return data.get();
        }

        const char* get() const {
            return data.get();
        }
    };

    template<class T>
    size_t byteSize(const std::vector<T>& vec) {
        return vec.size() * sizeof(T);
    }

}

#endif //WAGL2_UTILS_H
