#include <string>
#ifndef CONSOLEGO_BOARD_H
#define CONSOLEGO_BOARD_H

#endif //CONSOLEGO_BOARD_H

struct Board {
    int size;
    std::string player;
    void mutor_check(std::string key) {

    };
    bool LegalColour(std::string color, int size) {
        return true;
    }
};