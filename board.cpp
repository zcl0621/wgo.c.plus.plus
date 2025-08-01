//
// Created by 张朝龙 on 25-8-2.
//
#include <iomanip>
#include <iostream>
#include "position.cpp"

class Board {
public:
    int size;
    explicit Board(const int size) : size(size) {};

    void display(const Position &p) const {
        // Top coordinates
        std::cout << "";
        for (int x = 0; x < size+2; ++x) {

            std::cout << "-" << " ";
        }
        std::cout << std::endl;

        // Board rows
        for (int y = 0; y < size; ++y) {
            // Left coordinate with proper spacing for 2-digit numbers
            std::cout << std::setw(2) << "| ";
            
            // Board content
            for (int x = 0; x < size; ++x) {
                const auto c = p.get(x, y);
                if (c == BLACK) {
                    std::cout << "● ";
                } else if (c == WHITE) {
                    std::cout << "○ ";
                } else {
                    std::cout << "+ ";
                }
            }
            // Right coordinate
            std::cout  << std::setw(2) << "| " << std::endl;
        }

        // Bottom coordinates
        for (int x = 0; x < size+2; ++x) {
            std::cout <<  "- ";
        }
        std::cout << std::endl;
    }
};
