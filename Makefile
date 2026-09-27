CXX ?= g++
CXXFLAGS ?= -std=c++11 -Iinclude
LDLIBS ?= -lncursesw
PREFIX ?= /usr/local
INCLUDEDIR := $(PREFIX)/include
INSTALL ?= install

.PHONY: all clean install uninstall

all: example example_unicode example_windows_menus

example: example.cpp include/conio.hpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDLIBS)

example_unicode: example_unicode.cpp include/conio.hpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDLIBS)

example_windows_menus: example_windows_menus.cpp include/conio.hpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDLIBS)

install: include/conio.hpp
	mkdir -p "$(INCLUDEDIR)"
	$(INSTALL) -m 0644 include/conio.hpp "$(INCLUDEDIR)/conio.hpp"

uninstall:
	$(RM) "$(INCLUDEDIR)/conio.hpp"

clean:
	$(RM) example example_unicode example_windows_menus
