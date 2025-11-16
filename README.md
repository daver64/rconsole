# rconsole - Cross-Platform Console I/O Library

A modern, header-only C++ library for cross-platform console I/O operations. Provides a unified interface for console manipulation on both Linux (using ncurses) and Windows (using native Console API).

## Features

- **Header-only** - Just include `conio.hpp` and you're ready to go
- **Cross-platform** - Works on Linux (ncurses) and Windows (native Console API)
- **Simple namespace** - All functions in the `conio` namespace
- **Rich functionality**:
  - Cursor positioning (`gotoxy`)
  - Text output with positioning (`printf`, `putch`)
  - Colour support (16 colours: black, blue, green, cyan, red, magenta, yellow, white + bright variants)
  - Character input (`getchar`, `getcharecho`, `kbhit`)
  - Screen manipulation (`clrscr`, `getwidth`, `getheight`)
  - Cursor visibility control (`showcursor`)

## Requirements

### Linux
- C++11 or later
- ncurses library

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
    // Initialize console
    conio::init();
    
    // Clear screen
    conio::clrscr();
    
    // Move cursor and print
    conio::gotoxy(10, 5);
    conio::printf("Hello, World!");
    
    // Print with colour
    conio::textcolor(conio::Color::BRIGHT_GREEN);
    conio::gotoxy(10, 7);
    conio::printf("This is green text!");
    
    // Print at position with colour in one call
    conio::printf(10, 9, conio::Color::RED, "This is red text!");
    
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
g++ -std=c++11 example.cpp -o example -lncurses
```

Or with optimization:
```bash
g++ -std=c++11 -O2 example.cpp -o example -lncurses
```

### Windows

Using MSVC (Visual Studio):
```bash
cl /EHsc /std:c++11 example.cpp
```

Using MinGW:
```bash
g++ -std=c++11 example.cpp -o example.exe
```

## API Reference

### Initialization

```cpp
void conio::init()
```
Initializes the console. Must be called before using other functions.

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
void conio::textcolor(Color fg)
```
Sets the foreground (text) colour.

```cpp
void conio::textbackground(Color bg)
```
Sets the background colour.

```cpp
void conio::textattr(Color fg, Color bg)
```
Sets both foreground and background colours.

```cpp
void conio::resetattr()
```
Resets text attributes to default.

#### Available Colours

- `Color::BLACK`
- `Color::BLUE`
- `Color::GREEN`
- `Color::CYAN`
- `Color::RED`
- `Color::MAGENTA`
- `Color::YELLOW`
- `Color::WHITE`
- `Color::BRIGHT_BLACK`
- `Color::BRIGHT_BLUE`
- `Color::BRIGHT_GREEN`
- `Color::BRIGHT_CYAN`
- `Color::BRIGHT_RED`
- `Color::BRIGHT_MAGENTA`
- `Color::BRIGHT_YELLOW`
- `Color::BRIGHT_WHITE`

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
void conio::printf(int x, int y, Color fg, const char* format, ...)
```
Prints formatted text at specified position with foreground colour.

```cpp
void conio::printf(int x, int y, Color fg, Color bg, const char* format, ...)
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
void conio::putch(int x, int y, char c, Color fg)
```
Outputs a character at specified position with foreground colour.

```cpp
void conio::putch(int x, int y, char c, Color fg, Color bg)
```
Outputs a character at specified position with foreground and background colours.

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

## Example Program

Run the included example:

```bash
# Linux
g++ -std=c++11 example.cpp -o example -lncurses
./example

# Windows
cl /EHsc example.cpp
example.exe
```

The example demonstrates:
- Basic text positioning
- Colour usage (foreground and background)
- Character output with colours
- Drawing shapes (boxes)
- Console dimensions
- Animated graphics (bouncing ball)

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

Contributions are welcome! Feel free to submit issues and pull requests.

## Notes

- On Linux, the library uses ncurses which requires terminal support
- On Windows, the library uses the native Console API
- The library is thread-safe for initialization/cleanup but concurrent console operations from multiple threads may produce unexpected results
- Some colour combinations may appear differently depending on the terminal/console configuration

## Author

Dave R.

## Acknowledgments

- ncurses library for Linux console manipulation
- Windows Console API documentation
