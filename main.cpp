#include <iostream>
#include "board.cpp"
int main() {
    auto p = Position(19);
    p.set(0, 0, BLACK);
    p.set(0, 1, WHITE);
    const auto b = Board(19);
    b.display(p);
}
