# rconsole - Cross-Platform Console I/O Library

A modern, header-only C++ library for cross-platform console I/O operations. Provides a unified interface for console manipulation on both Linux (using ncurses) and Windows (using native Console API).

## Features

- **Header-only** - Just include `conio.hpp` and you're ready to go
- **Cross-platform** - Works on Linux (ncurses) and Windows (native Console API)
- **Simple namespace** - All functions in the `conio` namespace
- **Rich functionality**:
  - Cursor positioning (`gotoxy`)
  - Text output with positioning (`printf`, `putch`)
  - Unicode support (UTF-8 strings, wide characters with `putwch`, `wprintf`, `getwchar`)
  - Colour support (16 colours: black, blue, green, cyan, red, magenta, yellow, white + bright variants)
  - Character input (`getchar`, `getcharecho`, `kbhit`)
  - Screen manipulation (`clrscr`, `getwidth`, `getheight`)
  - Cursor visibility control (`showcursor`)

## Requirements

### Linux
- C++11 or later
- ncurses library (ncursesw for full Unicode support)

Install ncurses on Ubuntu/Debian:
```bash
sudo apt-get install libncurses-dev
```

Install ncurses on Fedora/RHEL:
```bash
sudo dnf install ncurses-devel
```

### Windows
- C++11 or later
- Windows SDK (included with Visual Studio)

## Usage

### Basic Example

```cpp
#include "conio.hpp"

int main() {
    // Initialise console
    conio::init();
    
    // Clear screen
    conio::clrscr();
    
    // Move cursor and print
    conio::gotoxy(10, 5);
    conio::printf("Hello, World!");
    
    // Print with colour
    conio::textcolour(conio::Colour::BRIGHT_GREEN);
    conio::gotoxy(10, 7);
    conio::printf("This is green text!");
    
    // Print at position with colour in one call
    conio::printf(10, 9, conio::Colour::RED, "This is red text!");
    
    // Wait for keypress
    conio::getchar();
    
    // Cleanup (optional - done automatically)
    conio::cleanup();
    
    return 0;
}
```

## Building

### Linux

Compile with ncurses:
```bash
g++ -std=c++11 -I include example.cpp -o example -lncurses
```

Compile with ncursesw for Unicode support:
```bash
g++ -std=c++11 -I include example.cpp -o example -lncursesw
```

Or with optimisation:
```bash
g++ -std=c++11 -O2 -I include example.cpp -o example -lncurses
```

### Windows

Using MSVC (Visual Studio):
```bash
cl /EHsc /std:c++11 /I include example.cpp
```

Using MinGW:
```bash
g++ -std=c++11 -I include example.cpp -o example.exe
```

## API Reference

### Initialization

```cpp
void conio::init()
```
Initialises the console. Must be called before using other functions.

```cpp
void conio::cleanup()
```
Cleans up console resources. Called automatically on exit.

### Cursor Control

```cpp
void conio::gotoxy(int x, int y)
```
Moves the cursor to position (x, y). Origin (0, 0) is top-left corner.

```cpp
void conio::showcursor(bool visible)
```
Shows or hides the cursor.

### Screen Operations

```cpp
void conio::clrscr()
```
Clears the screen.

```cpp
int conio::getwidth()
```
Returns the console width in characters.

```cpp
int conio::getheight()
```
Returns the console height in characters.

### Colour Control

```cpp
void conio::textcolour(Colour fg)
```
Sets the foreground (text) colour.

```cpp
void conio::textbackground(Colour bg)
```
Sets the background colour.

```cpp
void conio::textattr(Colour fg, Colour bg)
```
Sets both foreground and background colours.

```cpp
void conio::resetattr()
```
Resets text attributes to default.

#### Available Colours

- `Colour::BLACK`
- `Colour::BLUE`
- `Colour::GREEN`
- `Colour::CYAN`
- `Colour::RED`
- `Colour::MAGENTA`
- `Colour::YELLOW`
- `Colour::WHITE`
- `Colour::BRIGHT_BLACK`
- `Colour::BRIGHT_BLUE`
- `Colour::BRIGHT_GREEN`
- `Colour::BRIGHT_CYAN`
- `Colour::BRIGHT_RED`
- `Colour::BRIGHT_MAGENTA`
- `Colour::BRIGHT_YELLOW`
- `Colour::BRIGHT_WHITE`

### Text Output

```cpp
void conio::printf(const char* format, ...)
```
Prints formatted text at current cursor position.

```cpp
void conio::printf(int x, int y, const char* format, ...)
```
Prints formatted text at specified position.

```cpp
void conio::printf(int x, int y, Colour fg, const char* format, ...)
```
Prints formatted text at specified position with foreground colour.

```cpp
void conio::printf(int x, int y, Colour fg, Colour bg, const char* format, ...)
```
Prints formatted text at specified position with foreground and background colours.

### Character Output

```cpp
void conio::putch(char c)
```
Outputs a character at current cursor position.

```cpp
void conio::putch(int x, int y, char c)
```
Outputs a character at specified position.

```cpp
void conio::putch(int x, int y, char c, Colour fg)
```
Outputs a character at specified position with foreground colour.

```cpp
void conio::putch(int x, int y, char c, Colour fg, Colour bg)
```
Outputs a character at specified position with foreground and background colours.

### Unicode Output

```cpp
void conio::putwch(wchar_t wc)
```
Outputs a wide character (Unicode) at current cursor position.

```cpp
void conio::putwch(int x, int y, wchar_t wc)
```
Outputs a wide character at specified position.

```cpp
void conio::putwch(int x, int y, wchar_t wc, Colour fg)
```
Outputs a wide character at specified position with foreground colour.

```cpp
void conio::putwch(int x, int y, wchar_t wc, Colour fg, Colour bg)
```
Outputs a wide character at specified position with foreground and background colours.

```cpp
void conio::wprintf(const wchar_t* wstr)
```
Outputs a wide character string (Unicode) at current cursor position.

```cpp
void conio::wprintf(int x, int y, const wchar_t* wstr)
```
Outputs a wide character string at specified position.

```cpp
void conio::wprintf(int x, int y, Colour fg, const wchar_t* wstr)
```
Outputs a wide character string at specified position with foreground colour.

```cpp
void conio::wprintf(int x, int y, Colour fg, Colour bg, const wchar_t* wstr)
```
Outputs a wide character string at specified position with foreground and background colours.

```cpp
void conio::print_utf8(const char* utf8_str)
```
Outputs a UTF-8 encoded string at current cursor position.

```cpp
void conio::print_utf8(int x, int y, const char* utf8_str)
```
Outputs a UTF-8 encoded string at specified position.

```cpp
void conio::print_utf8(int x, int y, Colour fg, const char* utf8_str)
```
Outputs a UTF-8 encoded string at specified position with foreground colour.

```cpp
void conio::print_utf8(int x, int y, Colour fg, Colour bg, const char* utf8_str)
```
Outputs a UTF-8 encoded string at specified position with foreground and background colours.

### Character Input

```cpp
int conio::getchar()
```
Reads a character without echo (doesn't display the character).

```cpp
int conio::getcharecho()
```
Reads a character with echo (displays the character).

```cpp
bool conio::kbhit()
```
Checks if a key has been pressed (non-blocking).

```cpp
wint_t conio::getwchar()
```
Reads a wide character (Unicode input) without echo.

```cpp
wint_t conio::getwcharecho()
```
Reads a wide character (Unicode input) with echo.

## Example Program

Run the included examples:

```bash
# Linux - Basic example
g++ -std=c++11 -I include example.cpp -o example -lncurses
./example

# Linux - Unicode example (requires ncursesw)
g++ -std=c++11 -I include example_unicode.cpp -o example_unicode -lncursesw
./example_unicode

# Windows - Basic example
cl /EHsc /I include example.cpp
example.exe

# Windows - Unicode example
cl /EHsc /I include example_unicode.cpp
example_unicode.exe
```

The basic example demonstrates:
- Basic text positioning
- Colour usage (foreground and background)
- Character output with colours
- Drawing shapes (boxes)
- Console dimensions
- Animated graphics (bouncing ball)

The Unicode example demonstrates:
- UTF-8 emoji and symbols
- Unicode box drawing characters
- Mathematical symbols
- Greek letters
- Multiple language support (English, Spanish, French, German, Russian, Japanese, Chinese, Korean, Arabic)
- Unicode progress bars

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

Contributions are welcome! Feel free to submit issues and pull requests.

## Notes

- The API uses British English spelling (e.g., `textcolour` instead of `textcolor`)
- On Linux, the library uses ncursesw (wide character version) which requires terminal support for Unicode
- On Windows, the library uses the native Console API with UTF-8 code pages enabled
- Unicode support includes UTF-8 strings, wide character strings, emoji, box drawing characters, mathematical symbols, and multiple languages
- The library is thread-safe for initialisation/cleanup but concurrent console operations from multiple threads may produce unexpected results
- Some colour combinations may appear differently depending on the terminal/console configuration
- For best Unicode support, ensure your terminal/console is configured to use a UTF-8 locale

## Author

Dave R.

## Acknowledgments

- ncurses library for Linux console manipulation
- Windows Console API documentation
