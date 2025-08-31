#ifndef CONSOLEGO_BOARD_H
#define CONSOLEGO_BOARD_H


#include <string>

#include "colour.h"

struct Board {
    int size;
    Colour player;
    bool LegalColour(std::string p ,Colour color) { return true; }
};

#endif // CONSOLEGO_BOARD_H
