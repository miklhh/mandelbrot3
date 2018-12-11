#include <complex>
#include "render.h"
#include "filter.h"
#include <cmath>
#include <iostream>
#include <thread>

const mpfr::mpreal WIDTH = "0.000000000000001";
const mpfr::mpreal HEIGHT = "0.000000000000001";
const unsigned SCREEN_WIDTH = 640;
const unsigned SCREEN_HEIGHT = 480;
const unsigned MAX_ITERATIONS = 200000;

//const std::complex<mpfr::mpreal> CENTER = { "0.0", "0.0" };
const std::complex<mpfr::mpreal> CENTER = {
    "-1.74995768370609350360221450607069970727110579726252077930242837820286008082972804887218672784431700831100544507655659531379747541999999995",
    "0.00000000000000000278793706563379402178294753790944364927085054500163081379043930650189386849765202169477470552201325772332454726999999995"
};

int main()
{
    // Set float precision and print data.
    mpfr::mpreal::set_default_prec(mpfr::digits2bits(300));
    const std::size_t THREADS = std::thread::hardware_concurrency();
    const mpfr::mpreal eps = std::numeric_limits<mpfr::mpreal>::epsilon();
    std::cout << "Float - Epsilon: " << eps << std::endl;
    std::cout << "Float - Precision: " << eps * std::numeric_limits<mpfr::mpreal>::radix << std::endl;
    std::cout << "Float - Binary digits: " << mpfr::mpreal::get_default_prec() << std::endl;
    std::cout << "System reported threads: " << THREADS << ". Using: " << THREADS << std::endl;
    std::cout << "Max filter iterations: " << MAX_ITERATIONS << std::endl;

    // Initialize the render and apply the filter.
    using namespace std::chrono;
    mandelbrot_ss4_setup(SCREEN_WIDTH, SCREEN_HEIGHT, WIDTH, HEIGHT, MAX_ITERATIONS);
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
    std::cout << "Stored to \"frac.bmp\". Exiting." << std::endl;
    return 0;
}
