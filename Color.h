#pragma once

struct Color {
    unsigned char a = 255;
    unsigned char b = 255;
    unsigned char g = 255;
    unsigned char r = 255;

    Color(unsigned char r_, unsigned char g_, unsigned char b_,
          unsigned char a_ = 255)
        : a(a_), b(b_), g(g_), r(r_) {}
    Color() = default;

    Color operator+(const Color& rhs) {
        return Color(std::min(r + rhs.r, 255), std::min(g + rhs.g, 255),
                     std::min(b + rhs.b, 255), std::min(a + rhs.a, 255));
    }

    Color operator*(double f) {
        // Note that we do not change a
        return Color(r * f, g * f, b * f, a);
    }
};
