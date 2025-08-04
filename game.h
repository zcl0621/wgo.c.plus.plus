//
// Created by 张朝龙 on 25-8-2.
//
#include <vector>

#include "board.h"
#include "position.h"

#include <optional>


class Game {
private:
    std::vector<Position> stack;

public:
    int size;
    Color turn = BLACK;

    explicit Game(const int size) : size(size) {
        stack.emplace_back(size, 0, 0);
    };

    [[nodiscard]] Position &getPosition() { return stack.back(); }

    [[nodiscard]] const Position &getPosition() const { return stack.back(); }

    void setPosition(const Position &p) { stack.push_back(p); };

    void doCapture(Position &position, std::vector<Stone> &captured, int x, int y, Color c) {
        if (isOnBoard(x, y) && position.get(x, y) == c) {
            position.set(x, y, EMPTY);
            captured.emplace_back(x, y, c); // 使用 Stone 类

            doCapture(position, captured, x, y - 1, c); // Up
            doCapture(position, captured, x, y + 1, c); // Down
            doCapture(position, captured, x - 1, y, c); // Left
            doCapture(position, captured, x + 1, y, c); // Right
        }
    }

    bool checkLiberties(const Position &position, Position &testing, const int x, const int y, const Color c) {
        if (!isOnBoard(x, y)) {
            return true;
        }
        if (position.get(x, y) == EMPTY) {
            return false;
        }
        if (testing.get(x, y) == TESTED || position.get(x, y) == static_cast<Color>(-c)) {
            return true;
        }

        testing.set(x, y, TESTED);

        return checkLiberties(position, testing, x, y - 1, c) && checkLiberties(position, testing, x, y + 1, c) &&
               checkLiberties(position, testing, x - 1, y, c) && checkLiberties(position, testing, x + 1, y, c);
    }

    std::vector<Stone> checkCapturing(Position &position, int x, int y, Color c) {
        std::vector<Stone> captured; // 使用 Stone 类

        if (isOnBoard(x, y) && position.get(x, y) == c) {
            Position testing(size);
            if (checkLiberties(position, testing, x, y, c)) {
                doCapture(position, captured, x, y, c);
            }
        }

        return captured;
    }

    bool checkHistory(const Position &position, int x, int y) {
        for (int i = stack.size() - 2; i >= 0; i--) {
            if (stack[i].get(x, y) == position.get(x, y)) {
                bool identical = true;

                for (int j = 0; j < size * size; j++) {
                    if (stack[i].schema[j] != position.schema[j]) {
                        identical = false;
                        break;
                    }
                }

                if (identical) {
                    return false; // Found repeated position
                }
            }
        }
        return true; // No repetition found
    };

    std::variant<std::vector<Stone>, int> play(int x, int y, Color c = EMPTY, bool noplay = false) {
        // 检查坐标有效性
        if (!isOnBoard(x, y))
            return 1;
        if (getPosition().get(x, y) != EMPTY)
            return 2;

        // 如果没有指定颜色，使用当前回合的颜色
        if (c == EMPTY)
            c = turn;

        // 克隆当前位置
        Position new_pos = getPosition().clone();
        new_pos.set(x, y, c);

        // Check for captures
        std::vector<Stone> captured;

        // Check all four directions for captures
        auto captured1 = checkCapturing(new_pos, x - 1, y, static_cast<Color>(-c));
        auto captured2 = checkCapturing(new_pos, x + 1, y, static_cast<Color>(-c));
        auto captured3 = checkCapturing(new_pos, x, y - 1, static_cast<Color>(-c));
        auto captured4 = checkCapturing(new_pos, x, y + 1, static_cast<Color>(-c));

        // Combine all captured stones
        captured.insert(captured.end(), captured1.begin(), captured1.end());
        captured.insert(captured.end(), captured2.begin(), captured2.end());
        captured.insert(captured.end(), captured3.begin(), captured3.end());
        captured.insert(captured.end(), captured4.begin(), captured4.end());

        // 检查自杀
        if (captured.empty()) {
            Position testing(size);
            if (checkLiberties(new_pos, testing, x, y, c)) {
                return 3; // 自杀不允许
            }
        }

        // 检查历史（重复局面）
        if (!checkHistory(new_pos, x, y)) {
            return 4; // 重复局面
        }

        // 如果只是检查有效性，不实际落子
        if (noplay)
            return captured;

        // 更新位置信息
        new_pos.color = c;
        new_pos.blackCapture = getPosition().blackCapture;
        new_pos.whiteCapture = getPosition().whiteCapture;

        // Update capture counts
        if (c == BLACK) {
            new_pos.blackCapture += captured.size();
        } else {
            new_pos.whiteCapture += captured.size();
        }

        // 保存位置
        pushPosition(new_pos);

        // 切换回合
        turn = static_cast<Color>(-c);
        return captured;
    }
    void pass(const Color c = EMPTY) {
        pushPosition();
        Color current_color = (c != EMPTY) ? c : turn;
        getPosition().color = current_color;
        turn = static_cast<Color>(-current_color);
    }

    bool isValid(int x, int y, Color c = EMPTY) {
        auto result = play(x, y, c, true);
        return std::holds_alternative<std::vector<Stone>>(result);
    }

    [[nodiscard]] bool isOnBoard(const int x, const int y) const { return x >= 0 && x < size && y >= 0 && y < size; };

    bool addStone(const int x, const int y, const Color c = EMPTY) {
        if (isOnBoard(x, y) && getPosition().get(x, y) == EMPTY) {
            getPosition().set(x, y, c);
            return true;
        }
        return false;
    }

    bool removeStone(const int x, const int y) {
        if (isOnBoard(x, y) && getPosition().get(x, y) != EMPTY) {
            getPosition().set(x, y, EMPTY);
            return true;
        }
        return false;
    };

    bool setStone(const int x, const int y, const Color c) {
        if (isOnBoard(x, y)) {
            getPosition().set(x, y, c);
            return true;
        }
        return false;
    };

    Color getStone(const int x, const int y) {
        if (isOnBoard(x, y)) {
            return getPosition().get(x, y);
        }
        return EMPTY;
    };

    Position &pushPosition(const std::optional<Position> &pos = std::nullopt) {
        if (!pos) {
            Position newPos = stack.back().clone();

            stack.push_back(std::move(newPos));
        } else {
            stack.push_back(*pos);
        }

        if (stack.back().color != EMPTY) {
            turn = static_cast<Color>(-stack.back().color);
        }

        return stack.back();
    }

    std::optional<Position> popPosition() {
        if (!stack.empty()) {
            Position old = std::move(stack.back());

            stack.pop_back();

            if (stack.empty()) {
                turn = BLACK;
            } else if (stack.back().color != EMPTY) {
                turn = static_cast<Color>(-stack.back().color);
            } else {
                turn = static_cast<Color>(-turn);
            }

            return old;
        }

        return std::nullopt;
    }

    void firstPosition() {
        stack.clear();
        stack.emplace_back(size, 0, 0);
    };

    [[nodiscard]] int getCaptureCount(const Color c) const {
        auto const pos = getPosition();
        if (c == BLACK) {
            return pos.blackCapture;
        } else if (c == WHITE) {
            return pos.whiteCapture;
        } else {
            return 0;
        }
    };
    std::vector<Stone> validatePosition() {
        Color c;
        int white = 0, black = 0;
        std::vector<Stone> captured;
        Position new_pos = getPosition().clone();

        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {
                c = getPosition().get(x, y);
                if (c != EMPTY) {
                    size_t p = captured.size();

                    // 检查四个方向的提子
                    auto captured1 = checkCapturing(new_pos, x - 1, y, static_cast<Color>(-c));
                    auto captured2 = checkCapturing(new_pos, x + 1, y, static_cast<Color>(-c));
                    auto captured3 = checkCapturing(new_pos, x, y - 1, static_cast<Color>(-c));
                    auto captured4 = checkCapturing(new_pos, x, y + 1, static_cast<Color>(-c));

                    // 合并所有被提的棋子
                    captured.insert(captured.end(), captured1.begin(), captured1.end());
                    captured.insert(captured.end(), captured2.begin(), captured2.end());
                    captured.insert(captured.end(), captured3.begin(), captured3.end());
                    captured.insert(captured.end(), captured4.begin(), captured4.end());

                    // 更新提子计数
                    if (c == BLACK) {
                        black += captured.size() - p;
                    } else {
                        white += captured.size() - p;
                    }
                }
            }
        }

        // 更新当前位置的提子计数
        getPosition().blackCapture += black;
        getPosition().whiteCapture += white;

        // 更新棋盘状态
        getPosition().schema = new_pos.schema;

        return captured;
    }
};
