#include <array>
#include <string>

// alpha表，和Go版一致
constexpr char alpha[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

#ifndef CONSOLEGO_UTILS_H
#define CONSOLEGO_UTILS_H

#include <string>
#include <tuple>
#include <vector>

// TODO: complete it
bool ValidPoint(std::string p, int size) { return false; }

std::tuple<int, int, bool> ParsePoint(std::string p, int size) { return std::make_tuple(0, 0, false); }

// TODO: complete it
bool IsStarPoint(std::string p, int size) { return false; }

std::string Point(int x, int y) {
    if (x < 0 || x >= 52 || y < 0 || y >= 52) {
        return "";
    }
    return byte_to_string(alpha[x]) + byte_to_string(alpha[y]);
}

std::string byte_to_string(char b) { return std::string(1, b); }

std::vector<std::string> AdjacentPoints(std::string p, int size) {
    auto [x, y, onboard] = ParsePoint(p, size);
    if (!onboard) {
        return {};
    }
    std::vector<std::string> ret;
    if (x > 0) {
        ret.push_back(Point(x - 1, y));
    }
    if (x < size - 1) {
        ret.push_back(Point(x + 1, y));
    }
    if (y > 0) {
        ret.push_back(Point(x, y - 1));
    }
    if (y < size - 1) {
        ret.push_back(Point(x, y + 1));
    }
    return ret;
}

#endif // CONSOLEGO_UTILS_H
