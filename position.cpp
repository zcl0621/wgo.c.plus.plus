//
// Created by 张朝龙 on 25-8-1.
//
#include <vector>
#include "color.h"
#include "stone.cpp"

struct Compare {
    std::vector<Stone> add;
    std::vector<Stone> remove;
};


class Position {
public:
    const int size;
    std::vector<Color> schema;

    explicit Position(const int size) : size(size) {
        for (int i = 0; i < size * size; i++) {
            schema.push_back(EMPTY);
        }
    }

    [[nodiscard]] Color get(const int x, const int y) const { return schema[x * size + y]; }

    void set(const int x, const int y, const Color c) { schema.at(x * size + y) = c; }

    void clear() {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                set(i, j, EMPTY);
            }
        }
    }
    [[nodiscard]] Position clone() const {
        Position p(size);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                p.set(i, j, get(i, j));
            }
        }
        return p;
    }

    [[nodiscard]] Compare compares(const Position &p) const {
        Compare c;
        for (int i = 0; i < size * size; i++) {
            if (schema[i] != EMPTY && p.schema[i] && !p.schema[i]) {
                c.remove.emplace_back(i / size, i % size, schema[i]);
            } else if (schema[i] != EMPTY && schema[i] != p.schema[i]) {
                c.add.emplace_back(i / size, i % size, schema[i]);
            }
        }
        return c;
    }
};
