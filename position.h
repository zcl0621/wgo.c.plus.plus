//
// Created by 张朝龙 on 25-8-1.
//
#include <vector>
#include "color.h"
#include "stone.h"
#pragma once

struct Compare {
    std::vector<Stone> add;
    std::vector<Stone> remove;
};


class Position {
public:
    const int size;
    std::vector<Color> schema;
    int blackCapture = 0;
    int whiteCapture = 0;
    Color color = EMPTY;

    explicit Position(const int size, const int bc = 0, const int wc = 0) : size(size) {
        blackCapture = bc;
        whiteCapture = wc;
        for (int i = 0; i < size * size; i++) {
            schema.push_back(EMPTY);
        }
    }

    [[nodiscard]] Color get(const int x, const int y) const {
        if (x < 0 || y < 0 || x >= size || y >= size)
            return EMPTY;
        return schema[x * size + y];
    }

    void set(const int x, const int y, const Color c) {
        if (x < 0 || y < 0 || x >= size || y >= size)
            return;
        schema.at(x * size + y) = c;
    }

    void setColor(const Color c) { color = c; }

    void clear() {
        for (int i = 0; i < size * size; i++) {
            schema[i] = EMPTY;
        }
    }
    [[nodiscard]] Position clone() const {
        Position p(size, blackCapture, whiteCapture);
        p.schema = schema;
        p.color = color;
        return p;
    }

    [[nodiscard]] Compare compares(const Position &p) const {
        Compare c;
        for (int i = 0; i < size * size; i++) {
            if (schema[i] != EMPTY && p.schema[i] == EMPTY) {
                c.remove.emplace_back(i / size, i % size, schema[i]);
            } else if (schema[i] != p.schema[i]) {
                c.add.emplace_back(i / size, i % size, p.schema[i]);
            }
        }
        return c;
    }
};
