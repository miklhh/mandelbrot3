#include "render.h"
#include <stdexcept>
#include <SFML/Graphics/Image.hpp>

using Real = mpfr::mpreal;
using Complex = std::complex<Real>;
using Filter = std::function<sf::Color(Complex)>;

Render::Render(unsigned width, unsigned height, std::size_t threads, Filter filter)
    : imageWidth(width), imageHeight(height), pool(threads), filter(filter)
{ 
    if (height == 0 || width == 0 || threads == 0) 
    {
        // Invalid arguments.
        throw std::invalid_argument("'Render::Render' invalid argument.");
    }
    else
    {
        // Create the image. That should be drawn to.
        fractalImage.create(width, height);
    }
}

void Render::store(const std::string &filename) const
{
    fractalImage.saveToFile(filename);
}

bool Render::testComplete() const noexcept
{
    return !pool.working();
}

std::size_t Render::getRemainingSegments() const noexcept
{
    return pool.jobs();
}

void Render::applyFilter(const Complex &center, Real width, Real height)
{
    if (width == 0 || height == 0)
    {
        // Invalid arguments.
        throw std::invalid_argument("'Render::applyFilter' invalid argument.");
    }
    else
    {
        // Helper functions for the render segment function.
        auto getReal = [=](std::size_t px_x) -> Real
        {
            Real pxWidth = width / imageWidth;
            return center.real() - width / 2 + pxWidth * px_x;
        };
        auto getImag = [=](std::size_t px_y) -> Real
        {
            Real pxHeight = height / imageHeight;
            return center.imag() + height / 2 - pxHeight * px_y;
        };

        // Function for applying the filter to a smaller segment of the image.
        auto renderSegment = [=](std::size_t x_start, std::size_t x_end, 
                                 std::size_t y_start, std::size_t y_end)
        {
            for (std::size_t y = y_start; y < y_end; ++y)
                for (std::size_t x = x_start; x < x_end; ++x)
                {
                    Complex cmplx{ getReal(x), getImag(y) };
                    fractalImage.setPixel(x, y, filter(cmplx));
                }
        };

        // Enqueue all segments in the pool.
        const unsigned SEG_WIDTH = 4;
        const unsigned SEG_HEIGHT = 4;
        for (unsigned seg_x = 0; seg_x < imageWidth ; seg_x += SEG_WIDTH)
        {   
            unsigned x_len = imageWidth < seg_x + SEG_WIDTH ? imageWidth % SEG_WIDTH : SEG_WIDTH;
            for (unsigned seg_y = 0; seg_y < imageHeight; seg_y += SEG_HEIGHT)
            {
                unsigned y_len = imageHeight < seg_y + SEG_HEIGHT ? imageHeight % SEG_HEIGHT : SEG_HEIGHT;
                auto segment = std::bind(renderSegment, seg_x, seg_x + x_len, seg_y, seg_y + y_len);
                pool.enqueue(segment);
            }
        }
    }
}
