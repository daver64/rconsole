#ifndef CONIO_HPP
#define CONIO_HPP

#include <cstdio>
#include <cstdarg>
#include <string>
#include <memory>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <conio.h>
#else
    #include <ncurses.h>
    #include <unistd.h>
    #include <termios.h>
#endif

namespace conio {

// Colour constants
enum class Color {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    YELLOW = 6,
    WHITE = 7,
    BRIGHT_BLACK = 8,
    BRIGHT_BLUE = 9,
    BRIGHT_GREEN = 10,
    BRIGHT_CYAN = 11,
    BRIGHT_RED = 12,
    BRIGHT_MAGENTA = 13,
    BRIGHT_YELLOW = 14,
    BRIGHT_WHITE = 15
};

// RAII wrapper for console initialization
class Console {
private:
#ifdef _WIN32
    HANDLE hConsole;
    WORD defaultAttrs;
#else
    bool initialized;
#endif

public:
    Console() {
#ifdef _WIN32
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        defaultAttrs = csbi.wAttributes;
#else
        initscr();
        start_color();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(1);
        initialized = true;
        
        // Initialize colour pairs (foreground, background)
        // Pairs 1-15 for each foreground colour with black background
        for (int i = 0; i < 8; i++) {
            init_pair(i + 1, i, COLOR_BLACK);
        }
#endif
    }

    ~Console() {
#ifdef _WIN32
        SetConsoleTextAttribute(hConsole, defaultAttrs);
#else
        if (initialized) {
            endwin();
        }
#endif
    }

    // Prevent copying
    Console(const Console&) = delete;
    Console& operator=(const Console&) = delete;
};

// Global console instance - users should create one at the start of their program
inline std::unique_ptr<Console>& get_console() {
    static std::unique_ptr<Console> console_instance;
    return console_instance;
}

// Initialize console (must be called before using other functions)
inline void init() {
    get_console().reset(new Console());
}

// Cleanup console (automatically called on exit if using init())
inline void cleanup() {
    get_console().reset();
}

// Move cursor to position (0,0 is top-left)
inline void gotoxy(int x, int y) {
#ifdef _WIN32
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    move(y, x);
    refresh();
#endif
}

// Clear screen
inline void clrscr() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD cellCount;
    DWORD count;
    COORD homeCoord = {0, 0};

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputCharacter(hConsole, ' ', cellCount, homeCoord, &count);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoord, &count);
    SetConsoleCursorPosition(hConsole, homeCoord);
#else
    clear();
    refresh();
#endif
}

// Set text colour
inline void textcolor(Color fg) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD attrs = (csbi.wAttributes & 0xF0) | static_cast<WORD>(fg);
    SetConsoleTextAttribute(hConsole, attrs);
#else
    int color_val = static_cast<int>(fg);
    if (color_val < 8) {
        attron(COLOR_PAIR(color_val + 1));
    } else {
        attron(COLOR_PAIR((color_val - 8) + 1) | A_BOLD);
    }
    refresh();
#endif
}

// Set background colour
inline void textbackground(Color bg) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD attrs = (csbi.wAttributes & 0x0F) | (static_cast<WORD>(bg) << 4);
    SetConsoleTextAttribute(hConsole, attrs);
#else
    int bg_val = static_cast<int>(bg) % 8;
    int fg_val = 0; // Will be set by next textcolor call
    // Note: ncurses colour pairs need to be initialized with both fg and bg
    // For simplicity, we reinitialize the current pair
    PAIR_NUMBER(A_COLOR);
#endif
}

// Set both foreground and background colours
inline void textattr(Color fg, Color bg) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD attrs = static_cast<WORD>(fg) | (static_cast<WORD>(bg) << 4);
    SetConsoleTextAttribute(hConsole, attrs);
#else
    int fg_val = static_cast<int>(fg) % 8;
    int bg_val = static_cast<int>(bg) % 8;
    int pair_num = fg_val + bg_val * 8 + 16; // Offset to avoid conflicts
    
    init_pair(pair_num, fg_val, bg_val);
    attron(COLOR_PAIR(pair_num));
    
    if (static_cast<int>(fg) >= 8) {
        attron(A_BOLD);
    }
    refresh();
#endif
}

// Reset text attributes to default
inline void resetattr() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
    attrset(A_NORMAL);
    refresh();
#endif
}

// Print character at current position
inline void putch(char c) {
#ifdef _WIN32
    _putch(c);
#else
    addch(c);
    refresh();
#endif
}

// Print character at specified position
inline void putch(int x, int y, char c) {
    gotoxy(x, y);
    putch(c);
}

// Print character at specified position with colour
inline void putch(int x, int y, char c, Color fg, Color bg) {
    gotoxy(x, y);
    textattr(fg, bg);
    putch(c);
}

// Print character at specified position with foreground colour
inline void putch(int x, int y, char c, Color fg) {
    gotoxy(x, y);
    textcolor(fg);
    putch(c);
}

// Get a character (non-blocking on some systems)
inline int getchar() {
#ifdef _WIN32
    return _getch();
#else
    int ch = ::getch();
    return ch;
#endif
}

// Get a character with echo
inline int getcharecho() {
#ifdef _WIN32
    return _getche();
#else
    echo();
    int ch = ::getch();
    noecho();
    return ch;
#endif
}

// Check if key has been pressed
inline bool kbhit() {
#ifdef _WIN32
    return _kbhit() != 0;
#else
    nodelay(stdscr, TRUE);
    int ch = ::getch();
    nodelay(stdscr, FALSE);
    
    if (ch != ERR) {
        ungetch(ch);
        return true;
    }
    return false;
#endif
}

// Printf at current position
inline void printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
#ifdef _WIN32
    vprintf(format, args);
#else
    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format, args);
    printw("%s", buffer);
    refresh();
#endif
    
    va_end(args);
}

// Printf at specified position
inline void printf(int x, int y, const char* format, ...) {
    gotoxy(x, y);
    
    va_list args;
    va_start(args, format);
    
#ifdef _WIN32
    vprintf(format, args);
#else
    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format, args);
    printw("%s", buffer);
    refresh();
#endif
    
    va_end(args);
}

// Printf at specified position with colour
inline void printf(int x, int y, Color fg, Color bg, const char* format, ...) {
    gotoxy(x, y);
    textattr(fg, bg);
    
    va_list args;
    va_start(args, format);
    
#ifdef _WIN32
    vprintf(format, args);
#else
    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format, args);
    printw("%s", buffer);
    refresh();
#endif
    
    va_end(args);
}

// Printf at specified position with foreground colour
inline void printf(int x, int y, Color fg, const char* format, ...) {
    gotoxy(x, y);
    textcolor(fg);
    
    va_list args;
    va_start(args, format);
    
#ifdef _WIN32
    vprintf(format, args);
#else
    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format, args);
    printw("%s", buffer);
    refresh();
#endif
    
    va_end(args);
}

// Get console width
inline int getwidth() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
    int width, height;
    getmaxyx(stdscr, height, width);
    return width;
#endif
}

// Get console height
inline int getheight() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    int width, height;
    getmaxyx(stdscr, height, width);
    return height;
#endif
}

// Show/hide cursor
inline void showcursor(bool visible) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = visible;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
#else
    curs_set(visible ? 1 : 0);
#endif
}

} // namespace conio

#endif // CONIO_HPP
