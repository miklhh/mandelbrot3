#include "hsl.h"

/*
 * Shift the range of a value to [ranger_lower, range_higher]. Useful forexample
 * when you want to shift a negative angle to a standard angle [0, 2*pi]. If the
 * conversion can't be done, the function will return after 1000 iterations. 
 */
static double shift_range(double value, double range_lower, double range_higher)
{
    const int max_shifts = 1000;
    const double shift_value = range_higher - range_lower;
    for (int i = 0; i < max_shifts; i++)
    {
        if      (value < range_lower)  { value += shift_value; }
        else if (value > range_higher) { value -= shift_value; }
        else return value;
    }
    return value;
}


/* 
 * Conversion from wikipedia: 
 * https://en.wikipedia.org/wiki/HSL_and_HSV#Converting_to_RGB 
 */
sf::Color hsl_to_rgb(hsl_t hsl)
{
    /* Make sure that the hue is in it's correct spectrum. */
    const double PI = 3.14159265358979;
    hsl.hue = shift_range(hsl.hue, 0, 2*PI);
    double hue_prime = hsl.hue / (PI / 3);
    double chroma = (double(1) - abs(2 * hsl.light - 1)) * hsl.saturation;
    double x = chroma * (1 - abs(fmod(hue_prime, 2) - 1));

    double r1 = 0;
    double g1 = 0;
    double b1 = 0;
         if (0 <= hue_prime && hue_prime <= 1) { r1 = chroma;  g1 = x;       b1 = 0;      }
    else if (1 <= hue_prime && hue_prime <= 2) { r1 = x;       g1 = chroma;  b1 = 0;      }
    else if (2 <= hue_prime && hue_prime <= 3) { r1 = 0;       g1 = chroma;  b1 = x;      }
    else if (3 <= hue_prime && hue_prime <= 4) { r1 = 0;       g1 = x;       b1 = chroma; }
    else if (4 <= hue_prime && hue_prime <= 5) { r1 = x;       g1 = 0;       b1 = chroma; }
    else if (5 <= hue_prime && hue_prime <= 6) { r1 = chroma;  g1 = 0;       b1 = x;      }

    double m = hsl.light - chroma / 2;
    uint8_t r = uint8_t((r1 + m) * 255);
    uint8_t g = uint8_t((g1 + m) * 255);
    uint8_t b = uint8_t((b1 + m) * 255);
    return sf::Color{ r, g, b, 0 };
}
