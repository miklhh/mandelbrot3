#ifndef _HSL_H
#define _HSL_H

#include "mpfrc++/mpreal.h"
#include <SFML/Graphics/Color.hpp>

struct hsl_t
{
    double hue;
    double saturation;
    double light;
};

// Get the rgb color from a given hue, saturation and light value.
sf::Color hsl_to_rgb(hsl_t hsl);

#endif
