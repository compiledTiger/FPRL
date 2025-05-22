#pragma once

struct Color {
    // RGBA8888 = 4 bytes = 4 chars
    // first alpha, then B G R
    unsigned char a = 255;
    unsigned char b = 255;
    unsigned char g = 255;
    unsigned char r = 255;

    Color(unsigned char r_, unsigned char g_, unsigned char b_,
          unsigned char a_ = 255)
        : a(a_), b(b_), g(g_), r(r_) {}
    Color() = default;

    Color operator+(const Color& rhs) {
        return Color(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
    }

    Color operator*(double f) {
        // Note that we do not change a
        return Color(double(r) * f, double(g) * f, double(b) * f, a);
    }

    bool operator==(const Color& rhs) const {
        return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
    }

    bool operator!=(const Color& rhs) const { return !(*this == rhs); }

    Color mean(const Color& rhs) {
        return Color((r + rhs.r) / 2, (g + rhs.g) / 2, (b + rhs.b) / 2,
                     (a + rhs.a) / 2);
    }
};
