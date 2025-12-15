//
// Created by Pranav Sukesh on 8/27/25.
//

#ifndef COLOR_H
#define COLOR_H

#include "../Math/vec3.h"

using color = vec3;

inline void write_color(std::ostream& out, const color& pixel_color)
{
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    int rByte = int(255.999 * r);
    int gByte = int(255.999 * g);
    int bByte = int(255.999 * b);

    out << rByte << ' ' << gByte << ' ' << bByte << '\n';
}

#endif //COLOR_H
