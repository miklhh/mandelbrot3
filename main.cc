#include <complex>
#include "render.h"
#include "filter.h"
#include <cmath>
#include <iostream>
#include <thread>

const unsigned SCREEN_WIDTH = 1280;
const unsigned SCREEN_HEIGHT = 720;
const mpfr::mpreal WIDTH = "2.0";
const mpfr::mpreal HEIGHT = "2.0";
const std::complex<mpfr::mpreal> CENTER = { "0.0", "0.0" };


int main()
{
    // Set float precision and print data.
    mpfr::mpreal::set_default_prec(mpfr::digits2bits(300));
    const std::size_t THREADS = std::thread::hardware_concurrency();
    const int base = std::numeric_limits<mpfr::mpreal>::radix;
    const mpfr::mpreal eps = std::numeric_limits<mpfr::mpreal>::epsilon();
    const mpfr::mpreal prec = eps * base;
    const int bdigits = mpfr::mpreal::get_default_prec();
    std::cout << "Threads: " << THREADS << std::endl;
    std::cout << "Float - Epsilon: " << eps << std::endl;
    std::cout << "Float - Base: " << base << std::endl;
    std::cout << "Float - Precision: " << prec << std::endl;
    std::cout << "Float - Binary digits: " << bdigits << std::endl;

    // Initialize the render and apply the filter.
    using namespace std::chrono;
    mandelbrot_ss4_setup(SCREEN_WIDTH, SCREEN_HEIGHT, WIDTH, HEIGHT, 400);
    Render render (SCREEN_WIDTH, SCREEN_HEIGHT, THREADS, mandelbrot_ss4);
    auto t1 = high_resolution_clock::now();
    render.applyFilter(CENTER, WIDTH, HEIGHT);
    while (!render.testComplete())
    {
        std::printf("\r");
        std::printf("Segments remaining: %5lu", render.getRemainingSegments());
        std::fflush(stdout);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    auto t2 = high_resolution_clock::now();
    auto time = std::chrono::duration_cast<milliseconds>(t2 - t1);
    std::cout << std::endl;
    std::cout << "Rendering time: " << time.count() << " ms." << std::endl;

    render.store("frac.bmp");
    std::cout << "Done." << std::endl;
    return 0;
}
