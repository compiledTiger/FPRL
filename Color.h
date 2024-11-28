#pragma once

struct Color {
    unsigned char a;
    unsigned char b;
    unsigned char g;
    unsigned char r;

    Color(unsigned char r_, unsigned char g_, unsigned char b_,
          unsigned char a_ = 255)
        : a(a_), b(b_), g(g_), r(r_) {}
};
