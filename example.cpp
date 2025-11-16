#include "conio.hpp"
#include <thread>
#include <chrono>

int main() {
    // Initialize the console
    conio::init();
    
    // Clear the screen
    conio::clrscr();
    
    // Hide cursor for cleaner output
    conio::showcursor(false);
    
    // Display title
    conio::textattr(conio::Colour::BRIGHT_YELLOW, conio::Colour::BLUE);
    conio::gotoxy(0, 0);
    conio::printf("                    CONIO Library Demo                    ");
    conio::resetattr();
    
    // Basic text output
    conio::gotoxy(2, 2);
    conio::printf("1. Basic text output at position (2, 2)");
    
    // Coloured text
    conio::gotoxy(2, 4);
    conio::textcolor(conio::Colour::GREEN);
    conio::printf("2. Green coloured text");
    
    conio::gotoxy(2, 5);
    conio::textcolor(conio::Colour::RED);
    conio::printf("3. Red coloured text");
    
    conio::gotoxy(2, 6);
    conio::textcolor(conio::Colour::BRIGHT_CYAN);
    conio::printf("4. Bright cyan coloured text");
    
    conio::resetattr();
    
    // Printf with position and colour
    conio::printf(2, 8, conio::Colour::BRIGHT_MAGENTA, "5. Printf with x, y, and colour parameters");;
    
    // Printf with position, foreground and background colour
    conio::printf(2, 9, conio::Colour::YELLOW, conio::Colour::RED, "6. Text with yellow fg and red bg");
    
    conio::resetattr();
    
    // Character output
    conio::gotoxy(2, 11);
    conio::printf("7. Character output: ");
    for (char c = 'A'; c <= 'Z'; c++) {
        conio::putch(c);
    }
    
    // Coloured characters at specific positions
    conio::gotoxy(2, 13);
    conio::printf("8. Coloured characters:");
    
    int x_start = 2;
    int y_pos = 14;
    
    conio::putch(x_start, y_pos, '*', conio::Colour::RED);
    conio::putch(x_start + 2, y_pos, '*', conio::Colour::GREEN);
    conio::putch(x_start + 4, y_pos, '*', conio::Colour::BLUE);
    conio::putch(x_start + 6, y_pos, '*', conio::Colour::YELLOW);
    conio::putch(x_start + 8, y_pos, '*', conio::Colour::MAGENTA);
    conio::putch(x_start + 10, y_pos, '*', conio::Colour::CYAN);
    conio::putch(x_start + 12, y_pos, '*', conio::Colour::WHITE);
    
    conio::putch(x_start + 14, y_pos, '#', conio::Colour::BRIGHT_RED, conio::Colour::BLACK);
    conio::putch(x_start + 16, y_pos, '#', conio::Colour::BRIGHT_GREEN, conio::Colour::BLACK);
    conio::putch(x_start + 18, y_pos, '#', conio::Colour::BRIGHT_BLUE, conio::Colour::BLACK);
    conio::putch(x_start + 20, y_pos, '#', conio::Colour::BRIGHT_YELLOW, conio::Colour::BLACK);
    
    // Drawing a box
    conio::resetattr();
    conio::gotoxy(2, 16);
    conio::printf("9. Drawing a simple box:");
    
    int box_x = 2;
    int box_y = 17;
    int box_width = 40;
    int box_height = 5;
    
    conio::textcolor(conio::Colour::BRIGHT_WHITE);
    
    // Top and bottom borders
    for (int x = box_x; x < box_x + box_width; x++) {
        conio::putch(x, box_y, '-');
        conio::putch(x, box_y + box_height - 1, '-');
    }
    
    // Left and right borders
    for (int y = box_y; y < box_y + box_height; y++) {
        conio::putch(box_x, y, '|');
        conio::putch(box_x + box_width - 1, y, '|');
    }
    
    // Corners
    conio::putch(box_x, box_y, '+');
    conio::putch(box_x + box_width - 1, box_y, '+');
    conio::putch(box_x, box_y + box_height - 1, '+');
    conio::putch(box_x + box_width - 1, box_y + box_height - 1, '+');
    
    // Text inside box
    conio::printf(box_x + 8, box_y + 2, conio::Colour::BRIGHT_GREEN, "Console I/O is awesome!");
    
    conio::resetattr();
    
    // Console dimensions
    int width = conio::getwidth();
    int height = conio::getheight();
    conio::gotoxy(2, 23);
    conio::printf("10. Console dimensions: %dx%d", width, height);
    
    // Animated demonstration
    conio::gotoxy(2, 25);
    conio::textcolor(conio::Colour::BRIGHT_YELLOW);
    conio::printf("11. Animated bouncing ball (5 seconds)...");
    conio::resetattr();
    
    int ball_x = 5;
    int ball_y = 27;
    int dx = 1;
    int dy = 1;
    int max_x = width - 2;
    int max_y = height - 3;
    
    auto start_time = std::chrono::steady_clock::now();
    
    while (true) {
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
        
        if (elapsed >= 5) break;
        
        // Clear old position
        conio::putch(ball_x, ball_y, ' ');
        
        // Update position
        ball_x += dx;
        ball_y += dy;
        
        // Bounce off walls
        if (ball_x <= 0 || ball_x >= max_x) dx = -dx;
        if (ball_y <= 26 || ball_y >= max_y) dy = -dy;
        
        // Draw ball
        conio::putch(ball_x, ball_y, 'O', conio::Colour::BRIGHT_RED);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // Clear ball
    conio::putch(ball_x, ball_y, ' ');
    
    // Final message
    conio::showcursor(true);
    conio::gotoxy(2, height - 2);
    conio::textcolor(conio::Colour::BRIGHT_WHITE);
    conio::printf("Press any key to exit...");
    conio::resetattr();
    
    // Wait for key press
    conio::getchar();
    
    // Cleanup (optional, will be done automatically on exit)
    conio::clrscr();
    conio::gotoxy(0, 0);
    conio::cleanup();
    
    return 0;
}
