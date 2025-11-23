#ifndef CONIO_HPP
#define CONIO_HPP

#include <cstdio>
#include <cstdarg>
#include <string>
#include <memory>
#include <clocale>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <conio.h>
#else
    #define _XOPEN_SOURCE_EXTENDED 1
    #include <ncursesw/ncurses.h>
    #include <unistd.h>
    #include <termios.h>
    #include <locale.h>
    #include <wchar.h>
#endif

namespace conio {

// Colour constants
enum class Colour {
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
        // Set UTF-8 code page for Unicode support
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
#else
        // Set locale for UTF-8 support before initializing ncurses
        setlocale(LC_ALL, "");
        
        initscr();
        start_color();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(1);
        initialized = true;
        
        // Initialize colour pairs (foreground, background)
        // Map our enum order to ncurses COLOR_ constants
        // Our enum: BLACK(0), BLUE(1), GREEN(2), CYAN(3), RED(4), MAGENTA(5), YELLOW(6), WHITE(7)
        // ncurses:  BLACK(0), RED(1),  GREEN(2), YELLOW(3), BLUE(4), MAGENTA(5), CYAN(6),  WHITE(7)
        init_pair(1, COLOR_BLACK, COLOR_BLACK);   // BLACK
        init_pair(2, COLOR_BLUE, COLOR_BLACK);    // BLUE
        init_pair(3, COLOR_GREEN, COLOR_BLACK);   // GREEN
        init_pair(4, COLOR_CYAN, COLOR_BLACK);    // CYAN
        init_pair(5, COLOR_RED, COLOR_BLACK);     // RED
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK); // MAGENTA
        init_pair(7, COLOR_YELLOW, COLOR_BLACK);  // YELLOW
        init_pair(8, COLOR_WHITE, COLOR_BLACK);   // WHITE
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
inline void textcolour(Colour fg) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD attrs = (csbi.wAttributes & 0xF0) | static_cast<WORD>(fg);
    SetConsoleTextAttribute(hConsole, attrs);
#else
    int colour_val = static_cast<int>(fg);
    bool is_bright = (colour_val >= 8);
    int base_colour = is_bright ? (colour_val - 8) : colour_val;
    
    attron(COLOR_PAIR(base_colour + 1));
    if (is_bright) {
        attron(A_BOLD);
    } else {
        attroff(A_BOLD);
    }
    refresh();
#endif
}

// Set background colour
inline void textbackground(Colour bg) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD attrs = (csbi.wAttributes & 0x0F) | (static_cast<WORD>(bg) << 4);
    SetConsoleTextAttribute(hConsole, attrs);
#else
    int bg_val = static_cast<int>(bg) % 8;
    int fg_val = 0; // Will be set by next textcolour call
    // Note: ncurses colour pairs need to be initialized with both fg and bg
    // For simplicity, we reinitialize the current pair
    PAIR_NUMBER(A_COLOR);
#endif
}

// Set both foreground and background colours
inline void textattr(Colour fg, Colour bg) {
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
inline void putch(int x, int y, char c, Colour fg, Colour bg) {
    gotoxy(x, y);
    textattr(fg, bg);
    putch(c);
}

// Print character at specified position with foreground colour
inline void putch(int x, int y, char c, Colour fg) {
    gotoxy(x, y);
    textcolour(fg);
    putch(c);
}

// Wide character (Unicode) support

// Print wide character at current position
inline void putwch(wchar_t wc) {
#ifdef _WIN32
    wprintf(L"%lc", wc);
    fflush(stdout);
#else
    addnwstr(&wc, 1);
    refresh();
#endif
}

// Print wide character at specified position
inline void putwch(int x, int y, wchar_t wc) {
    gotoxy(x, y);
    putwch(wc);
}

// Print wide character at specified position with foreground colour
inline void putwch(int x, int y, wchar_t wc, Colour fg) {
    gotoxy(x, y);
    textcolour(fg);
    putwch(wc);
}

// Print wide character at specified position with colour
inline void putwch(int x, int y, wchar_t wc, Colour fg, Colour bg) {
    gotoxy(x, y);
    textattr(fg, bg);
    putwch(wc);
}

// Print wide string (Unicode) at current position
inline void wprintf(const wchar_t* wstr) {
#ifdef _WIN32
    ::wprintf(L"%ls", wstr);
    fflush(stdout);
#else
    addnwstr(wstr, wcslen(wstr));
    refresh();
#endif
}

// Print wide string at specified position
inline void wprintf(int x, int y, const wchar_t* wstr) {
    gotoxy(x, y);
    wprintf(wstr);
}

// Print wide string at specified position with foreground colour
inline void wprintf(int x, int y, Colour fg, const wchar_t* wstr) {
    gotoxy(x, y);
    textcolour(fg);
    wprintf(wstr);
}

// Print wide string at specified position with colour
inline void wprintf(int x, int y, Colour fg, Colour bg, const wchar_t* wstr) {
    gotoxy(x, y);
    textattr(fg, bg);
    wprintf(wstr);
}

// Print UTF-8 string (for convenience)
inline void print_utf8(const char* utf8_str) {
#ifdef _WIN32
    // Windows: convert UTF-8 to wide chars and print
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, NULL, 0);
    if (wlen > 0) {
        wchar_t* wstr = new wchar_t[wlen];
        MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, wstr, wlen);
        wprintf(L"%ls", wstr);
        delete[] wstr;
        fflush(stdout);
    }
#else
    // Linux: ncurses with UTF-8 locale handles this directly
    addstr(utf8_str);
    refresh();
#endif
}

// Print UTF-8 string with foreground colour (no position)
inline void print_utf8(Colour fg, const char* utf8_str) {
    textcolour(fg);
    print_utf8(utf8_str);
}

// Print UTF-8 string at specified position
inline void print_utf8(int x, int y, const char* utf8_str) {
    gotoxy(x, y);
    print_utf8(utf8_str);
}

// Print UTF-8 string at specified position with foreground colour
inline void print_utf8(int x, int y, Colour fg, const char* utf8_str) {
    gotoxy(x, y);
    textcolour(fg);
    print_utf8(utf8_str);
}

// Print UTF-8 string at specified position with colour
inline void print_utf8(int x, int y, Colour fg, Colour bg, const char* utf8_str) {
    gotoxy(x, y);
    textattr(fg, bg);
    print_utf8(utf8_str);
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

// Get a wide character (Unicode input)
inline wint_t getwchar() {
#ifdef _WIN32
    wint_t wc = _getwch();
    return wc;
#else
    wint_t wc;
    get_wch(&wc);
    return wc;
#endif
}

// Get a wide character with echo
inline wint_t getwcharecho() {
#ifdef _WIN32
    wint_t wc = _getwche();
    return wc;
#else
    echo();
    wint_t wc;
    get_wch(&wc);
    noecho();
    return wc;
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
inline void printf(int x, int y, Colour fg, Colour bg, const char* format, ...) {
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
inline void printf(int x, int y, Colour fg, const char* format, ...) {
    gotoxy(x, y);
    textcolour(fg);
    
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
