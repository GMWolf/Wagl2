//
// Created by felix on 15/01/2019.
//

#ifndef WAGL2_FLATMAP_H
#define WAGL2_FLATMAP_H

#include <vector>
#include <algorithm>
#include <stdexcept>

namespace wagl {

    template<typename T, typename U>
    class Flatmap {
        std::vector<T> keys;
        std::vector<U> values;

    public:

        void insert(const T& key, const U& value);

        const U& operator[](const T& key);
        const U& at(const T& key);

        void erase(const T& key);

        size_t size();

    };

    template<typename T, typename U>
    const U &Flatmap<T, U>::operator[](const T &key) {
        for(int i = 0; i < keys.size(); i++) {
            if (keys[i] == key) {
                return values[i];
            }
        }

        keys.push_back(key);
        values.emplace_back();
        return values.back();
    }

    template<typename T, typename U>
    const U &Flatmap<T, U>::at(const T &key) {
        for(int i = 0; i < keys.size(); i++) {
            if (keys[i] == key) {
                return values[i];
            }
        }
        throw std::runtime_error("No such key");
    }



    template<typename T, typename U>
    void Flatmap<T, U>::insert(const T &key, const U &value) {
        if (std::find(keys.begin(), keys.end(), key) != keys.end()) {
            keys.push_back(key);
            values.push_back(value);
        }
    }

    template<typename T, typename U>
    void Flatmap<T, U>::erase(const T &key) {

        for(int i = 0; i < keys.size(); i++) {
            if (keys[i] == key) {
                keys.erase(keys.begin() + i);
                values.erase(values.begin() + i);
                break;
            }
        }

    }

    template<typename T, typename U>
    size_t Flatmap<T, U>::size() {
        return keys.size();
    }

}

#endif //WAGL2_FLATMAP_H
