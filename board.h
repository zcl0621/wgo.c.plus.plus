#ifndef CONSOLEGO_BOARD_H
#define CONSOLEGO_BOARD_H

#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "colour.h"
#include "utils.h"

struct BoardMove {};

struct Board {
    int size;
    Colour player;
    std::string ko;
    float km;
    int step;
    std::vector<std::vector<Colour>> state;
    std::map<Colour, int> captureBy;
    bool paused;
    std::vector<std::shared_ptr<BoardMove>> move;
    int bContinuePass;
    int wContinuePass;
    float bScore;
    float wScore;
    int controversyCount;
    std::vector<float> ownership;

    Board() = default;
    Board(const Board &) = delete;
    Board &operator=(const Board &) = delete;
    ~Board() = default;

    Board(int sz) :
        size(sz), player(Colour::BLACK), ko(""), km(0.0f), step(0), state(sz, std::vector<Colour>(sz, Colour::EMPTY)),
        paused(false), bContinuePass(0), wContinuePass(0), bScore(0.0f), wScore(0.0f), controversyCount(0) {
        if (sz < 1 || sz > 52) {
            throw std::invalid_argument("NewBoard(): bad size " + std::to_string(sz));
        }
        move.clear();
        captureBy[Colour::BLACK] = 0;
        captureBy[Colour::WHITE] = 0;
        for (int i = 0; i < sz * sz; i++) {
            ownership.push_back(0.0f);
        }
    };

    // Equals returns true if the two boards are the same, including ko status,
    // captures, and next player to move.

    bool Equals(const Board &other) {
        if (this->size != other.size || this->player != other.player || this->ko != other.ko) {
            return false;
        }
        if (this->captureBy[Colour::BLACK] != other.captureBy.at(Colour::BLACK) ||
            this->captureBy[Colour::WHITE] != other.captureBy.at(Colour::WHITE)) {
            return false;
        }
        for (int i = 0; i < this->state.size(); i++) {
            for (int j = 0; j < this->state[i].size(); j++) {
                if (this->state[i][j] != other.state[i][j]) {
                    return false;
                }
            }
        }
        return true;
    };

    // Copy returns a deep copy of the board.
    std::shared_ptr<Board> Copy() {
        auto ret = std::make_shared<Board>();
        ret->size = this->size;
        ret->player = this->player;
        ret->ko = this->ko;
        ret->km = this->km;
        ret->step = this->step;
        ret->state = this->state;
        ret->captureBy = this->captureBy;
        ret->paused = this->paused;
        ret->bContinuePass = this->bContinuePass;
        ret->wContinuePass = this->wContinuePass;
        ret->bScore = this->bScore;
        ret->wScore = this->wScore;
        ret->controversyCount = this->controversyCount;
        ret->ownership = this->ownership;
        // Deep copy of move vector
        for (const auto &m: this->move) {
            ret->move.push_back(m); // Assuming BoardMove is immutable or handled elsewhere
        }
        return ret;
    };

    // Get returns the colour at the specified point. The argument should be an SGF
    // coordinate, e.g. "dd".
    Colour Get(std::string p) {
        auto [x, y, onboard] = ParsePoint(p, this->size);
        if (!onboard) {
            throw std::invalid_argument("Get(): point not on board: " + p);
        }
        return this->state[y][x];
    }

    // getFast is for trusted input
    Colour getFast(std::string p) {
        int x = static_cast<int>(p[0]) - 97;
        int y = static_cast<int>(p[1]) - 97;
        if (p[0] <= 'Z') {
            x = static_cast<int>(p[0]) - 39;
        }
        if (p[1] <= 'Z') {
            y = static_cast<int>(p[1]) - 39;
        }
        return state[x][y];
    }

    // HasKo returns true if the board has a ko square, on which capture by the
    // current player to move is prohibited.
    bool HasKo() { return this->ko != ""; };

    // Dump prints the board, and some information about captures and next player.
    void Dump() {
        std::string s = this->String();
        s += "Captures: " + std::to_string(this->captureBy[Colour::BLACK]) + " by Black - " +
             std::to_string(this->captureBy[Colour::WHITE]) + " by White\n";
        s += "Next player: " + Word(this->player) + "\n";
        std::cout << s;
    };

    void DumpBoard() { this->Dump(); }

    // String returns an ASCII representation of the board.
    std::string String() {
        std::ostringstream b;
        int ko_x = -1, ko_y = -1;
        if (!this->ko.empty()) {
            auto [x, y, onboard] = ParsePoint(this->ko, this->size);
            if (onboard) {
                ko_x = x;
                ko_y = y;
            }
        }
        for (int y = 0; y < this->size; y++) {
            for (int x = 0; x < this->size; x++) {
                Colour c = this->state[x][y];
                if (c == Colour::BLACK) {
                    b << " X";
                } else if (c == Colour::WHITE) {
                    b << " O";
                } else if (ko_x == x && ko_y == y) {
                    b << " :";
                } else {
                    if (IsStarPoint(Point(x, y), this->size)) {
                        b << " *"; // C++无HoshiString，默认用*，可自定义
                    } else {
                        b << " .";
                    }
                }
            }
            b << "\n";
        }
        return b.str();
    }

    std::string koSquareFinder(std::string p) {
        std::vector<std::string> hits;
        for (auto &a: AdjacentPoints(p, this->size)) {
            if (this->Get(a) == Colour::EMPTY) {
                hits.push_back(a);
            }
        }
        if (hits.size() != 1) {
            throw std::invalid_argument("koSquareFinder(): bad point: " + p);
        }
        return hits[0];
    };

    void ClearKo() {

    };
    std::string String() { return ""; };
    // TODO: complete this function
    bool LegalColour(std::string p, Colour color) { return true; }
};

#endif // CONSOLEGO_BOARD_H
