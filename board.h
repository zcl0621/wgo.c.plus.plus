#ifndef CONSOLEGO_BOARD_H
#define CONSOLEGO_BOARD_H


#include <string>

struct Board {
    int size;
    std::string player;
    bool LegalColour(std::string color, int size) { return true; }
};

#endif // CONSOLEGO_BOARD_H
