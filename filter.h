#ifndef _FILTER_H
#define _FILTER_H

#include "mpfrc++/mpreal.h"
#include <complex>
#include <SFML/Graphics/Color.hpp>

// Mandelbrot fractal filter.
void      mandelbrot_ss4_setup(unsigned screen_width, unsigned screen_height, mpfr::mpreal width, mpfr::mpreal height, unsigned max_iterations) noexcept;
sf::Color mandelbrot_ss4(std::complex<mpfr::mpreal> cmplx) noexcept;

#endif
