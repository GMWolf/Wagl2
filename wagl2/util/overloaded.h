//
// Created by felix on 6/1/2019.
//

#ifndef WAGL2_OVERLOADED_H
#define WAGL2_OVERLOADED_H
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
#endif //WAGL2_OVERLOADED_H
