#include "conio.hpp"
#include <thread>
#include <chrono>

int main() {
    // Initialise the console
    conio::init();
    
    // Clear the screen
    conio::clrscr();
    
    // Hide cursor for cleaner output
    conio::showcursor(false);
    
    // Display title with Unicode box drawing characters
    conio::textattr(conio::Colour::BRIGHT_YELLOW, conio::Colour::BLUE);
    conio::gotoxy(0, 0);
    conio::print_utf8("              Unicode Console Demo ✨              ");
    conio::resetattr();
    
    // UTF-8 emoji and symbols
    conio::gotoxy(2, 2);
    conio::printf("1. UTF-8 Emoji Support:");
    conio::gotoxy(4, 3);
    conio::print_utf8("   🌟 Stars  🎨 Art  🚀 Rocket  💻 Computer  ✅ Check");
    
    conio::gotoxy(4, 4);
    conio::print_utf8(conio::Colour::BRIGHT_RED, "   ❤️ Heart  ");
    conio::print_utf8(conio::Colour::BRIGHT_GREEN, "💚 Green  ");
    conio::print_utf8(conio::Colour::BRIGHT_BLUE, "💙 Blue");
    
    // Unicode box drawing
    conio::gotoxy(2, 6);
    conio::printf("2. Unicode Box Drawing:");
    
    int box_x = 4;
    int box_y = 7;
    int box_width = 30;
    int box_height = 5;
    
    conio::textcolour(conio::Colour::BRIGHT_CYAN);
    
    // Top border
    conio::gotoxy(box_x, box_y);
    conio::putwch(L'╔');
    for (int i = 1; i < box_width - 1; i++) {
        conio::putwch(L'═');
    }
    conio::putwch(L'╗');
    
    // Middle rows
    for (int y = 1; y < box_height - 1; y++) {
        conio::gotoxy(box_x, box_y + y);
        conio::putwch(L'║');
        conio::gotoxy(box_x + box_width - 1, box_y + y);
        conio::putwch(L'║');
    }
    
    // Bottom border
    conio::gotoxy(box_x, box_y + box_height - 1);
    conio::putwch(L'╚');
    for (int i = 1; i < box_width - 1; i++) {
        conio::putwch(L'═');
    }
    conio::putwch(L'╝');
    
    // Text inside box
    conio::wputs(box_x + 5, box_y + 2, conio::Colour::BRIGHT_WHITE, L"Unicode ♥ Console!");
    
    conio::resetattr();
    
    // Mathematical symbols
    conio::gotoxy(2, 13);
    conio::printf("3. Mathematical Symbols:");
    conio::gotoxy(4, 14);
    conio::print_utf8("   π ≈ 3.14159  ∑ ∫ ∞ √ ∂ ∇ ≠ ≤ ≥ ± × ÷");
    
    // Greek letters
    conio::gotoxy(2, 16);
    conio::printf("4. Greek Letters:");
    conio::gotoxy(4, 17);
    conio::print_utf8("   α β γ δ ε ζ η θ ι κ λ μ ν ξ ο π ρ σ τ υ φ χ ψ ω");
    conio::gotoxy(4, 18);
    conio::print_utf8("   Α Β Γ Δ Ε Ζ Η Θ Ι Κ Λ Μ Ν Ξ Ο Π Ρ Σ Τ Υ Φ Χ Ψ Ω");
    
    // Multiple languages
    conio::gotoxy(2, 20);
    conio::printf("5. Multiple Languages:");
    conio::gotoxy(4, 21);
    conio::print_utf8(conio::Colour::BRIGHT_GREEN, "   English: Hello World!");
    conio::gotoxy(4, 22);
    conio::print_utf8(conio::Colour::BRIGHT_YELLOW, "   Spanish: ¡Hola Mundo!");
    conio::gotoxy(4, 23);
    conio::print_utf8(conio::Colour::BRIGHT_CYAN, "   French: Bonjour le monde!");
    conio::gotoxy(4, 24);
    conio::print_utf8(conio::Colour::BRIGHT_MAGENTA, "   German: Hallo Welt!");
    conio::gotoxy(4, 25);
    conio::print_utf8(conio::Colour::BRIGHT_RED, "   Russian: Привет мир!");
    conio::gotoxy(4, 26);
    conio::print_utf8(conio::Colour::BRIGHT_BLUE, "   Japanese: こんにちは世界！");
    conio::gotoxy(4, 27);
    conio::print_utf8(conio::Colour::BRIGHT_WHITE, "   Chinese: 你好世界！");
    conio::gotoxy(4, 28);
    conio::print_utf8(conio::Colour::BRIGHT_GREEN, "   Korean: 안녕하세요 세계!");
    conio::gotoxy(4, 29);
    conio::print_utf8(conio::Colour::BRIGHT_YELLOW, "   Arabic: مرحبا بالعالم!");
    
    // Progress bar with Unicode characters
    conio::resetattr();
    conio::gotoxy(2, 31);
    conio::printf("6. Unicode Progress Bar:");
    conio::gotoxy(4, 32);
    
    conio::textcolour(conio::Colour::BRIGHT_CYAN);
    for (int i = 0; i <= 20; i++) {
        conio::gotoxy(4 + i, 32);
        if (i == 0) {
            conio::putwch(L'▓');
        } else if (i < 20) {
            conio::putwch(L'▓');
        } else {
            conio::putwch(L'▓');
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    conio::gotoxy(26, 32);
    conio::print_utf8(" ✓ Complete!");
    
    conio::resetattr();
    
    // Final message
    int height = conio::getheight();
    conio::showcursor(true);
    conio::gotoxy(2, height - 2);
    conio::textcolour(conio::Colour::BRIGHT_WHITE);
    conio::printf("Press any key to exit...");
    conio::resetattr();
    
    // Wait for key press
    conio::getchar();
    
    // Cleanup
    conio::clrscr();
    conio::gotoxy(0, 0);
    conio::cleanup();
    
    return 0;
}
