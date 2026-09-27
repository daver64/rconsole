#include "conio.hpp"
#include <vector>
#include <string>

int main() {
    conio::init();
    conio::clrscr();
    conio::showcursor(false);

    conio::Window status(2, 1, 56, 5, conio::Colour::BRIGHT_WHITE,
                         conio::Colour::BLUE, true, "STATUS");
    status.draw();
    status.print(2, 2, "A cross-platform text-mode window.");

    std::vector<std::string> choices;
    choices.push_back("Open file");
    choices.push_back("Save file");
    choices.push_back("Quit");

    conio::Menu menu(2, 7, 30, choices, conio::Colour::WHITE,
                     conio::Colour::BLACK, conio::Colour::BLACK,
                     conio::Colour::BRIGHT_CYAN);
    int choice = menu.run();

    conio::showcursor(true);
    conio::clrscr();
    conio::gotoxy(2, 2);
    conio::printf("Selected item: %d", choice);
    conio::getchar();
    conio::cleanup();
    return 0;
}
