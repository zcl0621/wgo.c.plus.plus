#ifndef CONSOLEGO_COLOUR_H
#define CONSOLEGO_COLOUR_H

#include <string>

enum class Colour : int8_t {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2,
    PAUSED = 3
};

// 获取对方颜色
inline Colour Opponent(Colour c) {
    if (c == Colour::BLACK) return Colour::WHITE;
    if (c == Colour::WHITE) return Colour::BLACK;
    return Colour::EMPTY;
}

// 转为大写字母
inline std::string Upper(Colour c) {
    if (c == Colour::BLACK) return "B";
    if (c == Colour::WHITE) return "W";
    return "?";
}

// 转为小写字母
inline std::string Lower(Colour c) {
    if (c == Colour::BLACK) return "b";
    if (c == Colour::WHITE) return "w";
    return "?";
}

// 转为单词
inline std::string Word(Colour c) {
    if (c == Colour::BLACK) return "Black";
    if (c == Colour::WHITE) return "White";
    return "??";
}

#endif // CONSOLEGO_COLOUR_H
