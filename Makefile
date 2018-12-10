CC = g++
CFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O3 -pg
LINK = -lmpfr -lpthread -lsfml-graphics

SRC = main.cc hsl.cc render.cc mandelbrot.cc
HEADERS = hsl.h render.h filter.h

.PHONY: clean

mandelbrot: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(LINK) $(SRC) -o $@
