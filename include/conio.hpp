#ifndef CONIO_HPP
#define CONIO_HPP

#include <cstdio>
#include <cstdarg>
#include <string>
#include <memory>
#include <clocale>
#include <mutex>
#include <vector>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <io.h>
    // Declare Windows console functions
    extern "C" {
        int _kbhit(void);
        int _getch(void);
        int _getche(void);
        int _putch(int c);
        wint_t _getwch(void);
        wint_t _getwche(void);
    }
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

#ifndef _WIN32
namespace detail {

inline short to_ncurses_colour(Colour colour) {
    // Colour enum order differs from ncurses constants, so map explicitly.
    switch (static_cast<int>(colour) % 8) {
        case 0: return COLOR_BLACK;
        case 1: return COLOR_BLUE;
        case 2: return COLOR_GREEN;
        case 3: return COLOR_CYAN;
        case 4: return COLOR_RED;
        case 5: return COLOR_MAGENTA;
        case 6: return COLOR_YELLOW;
        case 7: return COLOR_WHITE;
        default: return COLOR_WHITE;
    }
}

} // namespace detail
#endif

// Global mutex for thread-safe console operations
inline std::mutex& get_console_mutex() {
    static std::mutex console_mutex;
    return console_mutex;
}

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
        hConsole = INVALID_HANDLE_VALUE;
        defaultAttrs = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE) {
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
                defaultAttrs = csbi.wAttributes;
            }
        }

        // Set UTF-8 code page for Unicode support
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
#else
        initialized = false;

        // Set locale for UTF-8 support before initializing ncurses
        setlocale(LC_ALL, "");

        if (initscr() == nullptr) {
            return;
        }
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
        if (hConsole != INVALID_HANDLE_VALUE) {
            SetConsoleTextAttribute(hConsole, defaultAttrs);
        }
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

// Check if console is initialized
inline bool is_initialized() {
    return get_console() != nullptr;
}

// Initialize console (must be called before using other functions)
inline void init() {
    std::lock_guard<std::mutex> lock(get_console_mutex());
    get_console().reset(new Console());
}

// Cleanup console (automatically called on exit if using init())
inline void cleanup() {
    std::lock_guard<std::mutex> lock(get_console_mutex());
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
    if (hConsole == INVALID_HANDLE_VALUE) return;
    
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
    
    DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
    DWORD count;
    COORD homeCoord = {0, 0};

    FillConsoleOutputCharacter(hConsole, ' ', cellCount, homeCoord, &count);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoord, &count);
    SetConsoleCursorPosition(hConsole, homeCoord);
#else
    attr_t attrs = 0;
    short pair = 0;
    attr_get(&attrs, &pair, nullptr);
    wbkgd(stdscr, pair > 0 ? COLOR_PAIR(pair) : 0);
    clear();
    refresh();
#endif
}

// Set text colour
inline void textcolour(Colour fg) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return;
    
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
    
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
    if (hConsole == INVALID_HANDLE_VALUE) return;
    
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
    
    WORD attrs = (csbi.wAttributes & 0x0F) | (static_cast<WORD>(bg) << 4);
    SetConsoleTextAttribute(hConsole, attrs);
#else
    attr_t attrs = 0;
    short pair = 0;
    short current_fg = COLOR_WHITE;
    short current_bg = COLOR_BLACK;

    if (attr_get(&attrs, &pair, nullptr) == OK && pair > 0) {
        short fg = COLOR_WHITE;
        short bg_current = COLOR_BLACK;
        if (pair_content(pair, &fg, &bg_current) == OK) {
            current_fg = fg;
            current_bg = bg_current;
        }
    }

    short bg_val = detail::to_ncurses_colour(bg);
    int pair_num = 1 + (bg_val % 8) * 8 + (current_fg % 8);
    if (pair_num >= COLOR_PAIRS) {
        pair_num = 1 + (current_bg % 8) * 8 + (current_fg % 8);
    }
    if (pair_num >= COLOR_PAIRS) {
        pair_num = 1;
    }

    init_pair(static_cast<short>(pair_num), current_fg, bg_val);
    attron(COLOR_PAIR(pair_num));
    refresh();
#endif
}

// Set both foreground and background colours
inline void textattr(Colour fg, Colour bg) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return;
    
    WORD attrs = static_cast<WORD>(fg) | (static_cast<WORD>(bg) << 4);
    SetConsoleTextAttribute(hConsole, attrs);
#else
    int fg_val = detail::to_ncurses_colour(fg);
    int bg_val = detail::to_ncurses_colour(bg);
    // Keep pair numbering in a compact 8x8 matrix.
    int pair_num = 1 + bg_val * 8 + fg_val;

    if (pair_num > 0 && pair_num < COLOR_PAIRS) {
        init_pair(pair_num, fg_val, bg_val);
        attron(COLOR_PAIR(pair_num));
    }

    if (static_cast<int>(fg) >= 8) {
        attron(A_BOLD);
    } else {
        attroff(A_BOLD);
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
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;
    WriteConsoleW(hConsole, &wc, 1, &written, NULL);
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
inline void wputs(const wchar_t* wstr) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;
    WriteConsoleW(hConsole, wstr, static_cast<DWORD>(wcslen(wstr)), &written, NULL);
#else
    addnwstr(wstr, wcslen(wstr));
    refresh();
#endif
}

// Print wide string at specified position
inline void wputs(int x, int y, const wchar_t* wstr) {
    gotoxy(x, y);
    wputs(wstr);
}

// Print wide string at specified position with foreground colour
inline void wputs(int x, int y, Colour fg, const wchar_t* wstr) {
    gotoxy(x, y);
    textcolour(fg);
    wputs(wstr);
}

// Print wide string at specified position with colour
inline void wputs(int x, int y, Colour fg, Colour bg, const wchar_t* wstr) {
    gotoxy(x, y);
    textattr(fg, bg);
    wputs(wstr);
}

// Print UTF-8 string (for convenience)
inline void print_utf8(const char* utf8_str) {
#ifdef _WIN32
    // Windows: convert UTF-8 to wide chars and print
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, NULL, 0);
    if (wlen > 0) {
        std::unique_ptr<wchar_t[]> wstr(new wchar_t[wlen]);
        MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, wstr.get(), wlen);
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD written;
        WriteConsoleW(hConsole, wstr.get(), static_cast<DWORD>(wcslen(wstr.get())), &written, NULL);
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
    // Note: ncurses input in raw mode may need special handling
    // For now, simply enable echo, read, and disable echo
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
    // Note: ncurses input in raw mode may need special handling
    // For now, simply enable echo, read, and disable echo
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

// Helper function for printf operations
inline void vprintf_impl(const char* format, va_list args) {
#ifdef _WIN32
    vprintf(format, args);
#else
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);

    if (needed < 0) {
        return;
    }

    std::string buffer(static_cast<size_t>(needed) + 1, '\0');
    vsnprintf(&buffer[0], buffer.size(), format, args);
    printw("%s", buffer.c_str());
    refresh();
#endif
}

// Printf at current position
inline void printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf_impl(format, args);
    va_end(args);
}

// Printf at specified position
inline void printf(int x, int y, const char* format, ...) {
    gotoxy(x, y);
    
    va_list args;
    va_start(args, format);
    vprintf_impl(format, args);
    va_end(args);
}

// Printf at specified position with colour
inline void printf(int x, int y, Colour fg, Colour bg, const char* format, ...) {
    gotoxy(x, y);
    textattr(fg, bg);
    
    va_list args;
    va_start(args, format);
    vprintf_impl(format, args);
    va_end(args);
}

// Printf at specified position with foreground colour
inline void printf(int x, int y, Colour fg, const char* format, ...) {
    gotoxy(x, y);
    textcolour(fg);
    
    va_list args;
    va_start(args, format);
    vprintf_impl(format, args);
    va_end(args);
}

// Get console width
inline int getwidth() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return 80; // Default width
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return 80;
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
    int width = 0, height = 0;
    getmaxyx(stdscr, height, width);
    (void)height; // height is only needed for getmaxyx macro
    return width > 0 ? width : 80;
#endif
}

// Get console height
inline int getheight() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return 24; // Default height
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return 24;
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

class Window {
private:
    int x;
    int y;
    int width;
    int height;
    Colour foreground;
    Colour background;
    bool bordered;
    std::string title;
#ifndef _WIN32
    WINDOW* native_window;
#endif

    void draw_border() {
        if (!bordered) {
            return;
        }
#ifdef _WIN32
        for (int column = 0; column < width; ++column) {
            putch(x + column, y, '-');
            putch(x + column, y + height - 1, '-');
        }
        for (int row = 1; row < height - 1; ++row) {
            putch(x, y + row, '|');
            putch(x + width - 1, y + row, '|');
        }
        putch(x, y, '+');
        putch(x + width - 1, y, '+');
        putch(x, y + height - 1, '+');
        putch(x + width - 1, y + height - 1, '+');
#else
        box(native_window, 0, 0);
#endif
    }

public:
    Window(int window_x, int window_y, int window_width, int window_height,
           Colour window_foreground = Colour::WHITE,
           Colour window_background = Colour::BLACK,
           bool window_bordered = true,
           const std::string& window_title = std::string())
        : x(window_x), y(window_y), width(window_width), height(window_height),
          foreground(window_foreground), background(window_background),
          bordered(window_bordered), title(window_title)
#ifndef _WIN32
          , native_window(nullptr)
#endif
    {
        if (width < 2) width = 2;
        if (height < 2) height = 2;
#ifndef _WIN32
        native_window = newwin(height, width, y, x);
#endif
    }

    ~Window() {
#ifndef _WIN32
        if (native_window != nullptr) {
            delwin(native_window);
        }
#endif
    }

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void clear() {
#ifdef _WIN32
        textattr(foreground, background);
        for (int row = 0; row < height; ++row) {
            for (int column = 0; column < width; ++column) {
                putch(x + column, y + row, ' ');
            }
        }
#else
        int pair = 1 + detail::to_ncurses_colour(background) * 8 +
                   detail::to_ncurses_colour(foreground);
        if (pair > 0 && pair < COLOR_PAIRS) {
            init_pair(pair, detail::to_ncurses_colour(foreground),
                      detail::to_ncurses_colour(background));
            wbkgd(native_window, COLOR_PAIR(pair));
        }
        werase(native_window);
#endif
    }

    void draw() {
        clear();
#ifdef _WIN32
        textattr(foreground, background);
#else
        int pair = 1 + detail::to_ncurses_colour(background) * 8 +
                   detail::to_ncurses_colour(foreground);
        if (pair > 0 && pair < COLOR_PAIRS) {
            wattron(native_window, COLOR_PAIR(pair));
        }
#endif
        draw_border();
        if (!title.empty() && width > 4) {
            print(2, 0, title.c_str());
        }
#ifdef _WIN32
        gotoxy(x, y);
#else
        wrefresh(native_window);
#endif
    }

    void print(int relative_x, int relative_y, const char* text) {
        if (text == nullptr || relative_x < 0 || relative_y < 0 ||
            relative_x >= width || relative_y >= height) {
            return;
        }
#ifdef _WIN32
        textattr(foreground, background);
        gotoxy(x + relative_x, y + relative_y);
        for (int index = 0; text[index] != '\0' && relative_x + index < width; ++index) {
            putch(text[index]);
        }
#else
        mvwaddnstr(native_window, relative_y, relative_x, text,
                   width - relative_x);
        wrefresh(native_window);
#endif
    }

    void print(int relative_x, int relative_y, Colour text_foreground,
               Colour text_background, const char* text) {
        if (text == nullptr || relative_x < 0 || relative_y < 0 ||
            relative_x >= width || relative_y >= height) {
            return;
        }
#ifdef _WIN32
        textattr(text_foreground, text_background);
        gotoxy(x + relative_x, y + relative_y);
        for (int index = 0; text[index] != '\0' && relative_x + index < width; ++index) {
            putch(text[index]);
        }
#else
        int foreground_value = detail::to_ncurses_colour(text_foreground);
        int background_value = detail::to_ncurses_colour(text_background);
        int pair = 1 + background_value * 8 + foreground_value;
        if (pair > 0 && pair < COLOR_PAIRS) {
            init_pair(pair, foreground_value, background_value);
            wattron(native_window, COLOR_PAIR(pair));
        }
        mvwaddnstr(native_window, relative_y, relative_x, text,
                   width - relative_x);
        wattroff(native_window, COLOR_PAIR(pair));
        wrefresh(native_window);
#endif
    }

    int get_x() const { return x; }
    int get_y() const { return y; }
    int get_width() const { return width; }
    int get_height() const { return height; }
};

class Menu {
private:
    Window window;
    std::vector<std::string> items;
    Colour selected_foreground;
    Colour selected_background;
    int selected;

    void draw_items() {
        window.draw();
        for (size_t index = 0; index < items.size(); ++index) {
            int row = static_cast<int>(index) + 1;
            if (row >= window.get_height() - 1) {
                break;
            }
#ifdef _WIN32
            window.print(2, row,
                         index == static_cast<size_t>(selected) ? selected_foreground : Colour::WHITE,
                         index == static_cast<size_t>(selected) ? selected_background : Colour::BLACK,
                         items[index].c_str());
#else
            window.print(2, row,
                         index == static_cast<size_t>(selected) ? selected_foreground : Colour::WHITE,
                         index == static_cast<size_t>(selected) ? selected_background : Colour::BLACK,
                         items[index].c_str());
#endif
        }
    }

public:
    Menu(int menu_x, int menu_y, int menu_width, const std::vector<std::string>& menu_items,
         Colour menu_foreground = Colour::WHITE,
         Colour menu_background = Colour::BLACK,
         Colour menu_selected_foreground = Colour::BLACK,
         Colour menu_selected_background = Colour::WHITE)
        : window(menu_x, menu_y, menu_width,
                 static_cast<int>(menu_items.size()) + 3,
                 menu_foreground, menu_background),
          items(menu_items), selected_foreground(menu_selected_foreground),
          selected_background(menu_selected_background), selected(0) {}

    void draw() { draw_items(); }

    int run() {
        if (items.empty()) {
            return -1;
        }
        draw_items();
        for (;;) {
#ifdef _WIN32
            int key = getchar();
            if (key == 0 || key == 224) {
                key = getchar();
                if (key == 72) --selected;
                if (key == 80) ++selected;
            } else if (key == 13) {
                return selected;
            } else if (key == 27) {
                return -1;
            }
#else
            int key = wgetch(stdscr);
            if (key == KEY_UP) --selected;
            else if (key == KEY_DOWN) ++selected;
            else if (key == '\n' || key == KEY_ENTER) return selected;
            else if (key == 27) return -1;
#endif
            if (selected < 0) selected = static_cast<int>(items.size()) - 1;
            if (selected >= static_cast<int>(items.size())) selected = 0;
            draw_items();
        }
    }

    int selected_index() const { return selected; }
};

} // namespace conio

#endif // CONIO_HPP
