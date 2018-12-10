/*
 * Note: The filter function used for rendering the complex image has to be
 * threadsafe. The renderer will perform multiple parallel calls to the filter
 * function when rendering the image.
 */

#ifndef _RENDER_H
#define _RENDER_H

#include "threadpool/threadpool.h"
#include "mpfrc++/mpreal.h"
#include <complex>
#include <functional>
#include <string>
#include <SFML/Graphics/Image.hpp>

class Render
{
    using Real = mpfr::mpreal;
    using Complex = std::complex<Real>;
    using Filter = std::function<sf::Color(Complex)>;

public:
    Render(unsigned width, unsigned height, std::size_t threads, Filter filter);

    // Rendering methods.
    void store(const std::string &filename) const;
    bool testComplete() const noexcept;
    std::size_t getRemainingSegments() const noexcept;
    void applyFilter(const Complex &center, Real width, Real height);

private:
    sf::Image fractalImage{};
    unsigned imageWidth;
    unsigned imageHeight;
    ThreadPool pool;
    Filter filter;
};

#endif
