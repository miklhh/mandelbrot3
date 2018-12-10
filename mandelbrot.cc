#include "filter.h"
#include "hsl.h"
#include "mpfrc++/mpreal.h"
#include <cmath>

static unsigned max_iterations;
static unsigned screen_width{};
static unsigned screen_height{};
static mpfr::mpreal width{};
static mpfr::mpreal height{};


// Get a complex supersampled offset from a segment number.
static std::complex<mpfr::mpreal> getOffsetSs4(int segment)
{
    using Real = mpfr::mpreal;
    const Real OFFSET_WIDTH  = width  / screen_width  / 4;
    const Real OFFSET_HEIGHT = height / screen_height / 4;
    switch (segment)
    {
        case 0: return {  OFFSET_WIDTH,  OFFSET_HEIGHT }; break;
        case 1: return { -OFFSET_WIDTH,  OFFSET_HEIGHT }; break;
        case 2: return { -OFFSET_WIDTH, -OFFSET_HEIGHT }; break;
        case 3: return {  OFFSET_WIDTH, -OFFSET_HEIGHT }; break;
        default: throw std::invalid_argument("getOffsetSs4(int)"); break;
    }
}

// Returns the mean color of a sf::Color[] of 'n' elements.
static sf::Color getMeanColor(sf::Color color[], std::size_t n)
{
    sf::Color res{ 0, 0, 0 };
    for (std::size_t i = 0; i < n; ++i)
    {
        res.r += color[i].r / n;
        res.g += color[i].g / n;
        res.b += color[i].b / n;
    }
    return res;
}


// A convergence value is the value that would be feed to the color retrieveing
// funcion.
static mpfr::mpreal getConvergenceValue(
        unsigned iterations, 
        std::complex<mpfr::mpreal> zn,
        std::complex<mpfr::mpreal> c)
{
    using Real = mpfr::mpreal;
    const Real ESCAPE_RADIUS = 2.0;
    for (int i = 0; i < 5; ++i)
    {
        zn = zn * zn + c;
        iterations++;
    }
    return Real(iterations) - mpfr::log2( mpfr::log(std::abs(zn)) / mpfr::log(ESCAPE_RADIUS) );
}

// Get a mandelbrot color.
static sf::Color getColor(
        unsigned iterations,
        const std::complex<mpfr::mpreal> &zn,
        const std::complex<mpfr::mpreal> &c)
{
    using Real = mpfr::mpreal;
    if (iterations >= max_iterations)
    {
        // The complex point has escaped, it is inside the mandelbrot set.
        return sf::Color::Black;
    }
    else
    {
        // The complex point did not escape, it is outside the set. The 
        // getConvergenceValue function is used to preform a couple of extra
        // iterations, since this will give a more smooth coloroing.
        Real convergenceValue = getConvergenceValue(iterations, zn, c);
        return hsl_to_rgb( { static_cast<double>(mpfr::log(10 * convergenceValue)), 1, 0.7 } );
    }

}

static void ss4Test(sf::Color *segments, int segment, std::complex<mpfr::mpreal> &c)
{
    using Real = mpfr::mpreal;
    Real zReal = 0;
    Real zImag = 0;
    Real zRealSqr = sqr(zReal);
    Real zImagSqr = sqr(zImag);
    for (unsigned iteration = 0; iteration < max_iterations; ++iteration)
    {
        if (zRealSqr + zImagSqr > 4)
        {
            std::complex<Real> zn{ zReal, zImag };
            segments[segment] = getColor(iteration, zn, c);
            break;
        }
        zImag  = sqr(zReal + zImag) - zRealSqr - zImagSqr;
        zImag += c.imag();
        zReal  = zRealSqr - zImagSqr + c.real();
        zRealSqr  = sqr(zReal);
        zImagSqr  = sqr(zImag);
    }
}


// Setup the four times supersampler mandelbrot filter.
void mandelbrot_ss4_setup(unsigned sw, unsigned sh, mpfr::mpreal w, mpfr::mpreal h, unsigned max_it)
{
    screen_width = sw;
    screen_height = sh;
    width = w;
    height = h;
    max_iterations = max_it;
}

// Filter for rendering a four times supersampled mandelbrot fractal.
sf::Color mandelbrot_ss4(std::complex<mpfr::mpreal> cmplx)
{
    using Real = mpfr::mpreal;
    using Complex = std::complex<Real>;
    auto sqr = [](Real x)->Real { return x * x; };
    sf::Color segments[4]{};

    for (int segment = 0; segment < 4; ++segment)
    {
        // Adjust 'c' for use of supersampling.
        Complex c = cmplx + getOffsetSs4(segment);

        mpfr::mpreal x = c.real();
        mpfr::mpreal y = c.imag();
        mpfr::mpreal q = sqr(x - (Real)1/4) + sqr(y);
        if ( q*(q + x - (Real)1/4) < (Real)1/4 * sqr(y) )
        {
            // Main cardioid.
            segments[segment] = sf::Color::Black;
        }
        else if ( sqr(x + 1) + sqr(y) < (Real)1/16 )
        {
            // Period one bulb.
            segments[segment] = sf::Color::Black;
        }
        else
        {
            // Test requiered.
            ss4Test(segments, segment, c);
        }
    }
    return getMeanColor(segments, 4);
}
