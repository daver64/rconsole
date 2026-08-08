#include <cassert>
#include <iostream>

#include "conio.hpp"

int main() {
#ifndef _WIN32
    using conio::Colour;

    assert(conio::detail::to_ncurses_colour(Colour::BLACK) == COLOR_BLACK);
    assert(conio::detail::to_ncurses_colour(Colour::BLUE) == COLOR_BLUE);
    assert(conio::detail::to_ncurses_colour(Colour::GREEN) == COLOR_GREEN);
    assert(conio::detail::to_ncurses_colour(Colour::CYAN) == COLOR_CYAN);
    assert(conio::detail::to_ncurses_colour(Colour::RED) == COLOR_RED);
    assert(conio::detail::to_ncurses_colour(Colour::MAGENTA) == COLOR_MAGENTA);
    assert(conio::detail::to_ncurses_colour(Colour::YELLOW) == COLOR_YELLOW);
    assert(conio::detail::to_ncurses_colour(Colour::WHITE) == COLOR_WHITE);

    // Bright variants should map to the same base ncurses colour.
    assert(conio::detail::to_ncurses_colour(Colour::BRIGHT_BLUE) == COLOR_BLUE);
    assert(conio::detail::to_ncurses_colour(Colour::BRIGHT_RED) == COLOR_RED);
#endif

    std::cout << "colour_mapping_test: OK\n";
    return 0;
}
