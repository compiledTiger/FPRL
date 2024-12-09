#pragma once

template <int width_, int height_>
struct WallTexture {
    static const int width = width_;
    static const int height = height_;
    Color pixels[width * height];
    // walls() sets values to nullptr
    WallTexture() : pixels() {}
    Color& at(int x, int y) { return pixels[y * width + x]; }
    void generate() {
        for (auto& p : pixels) p = Color(160, 80, 40);

        for (int h = 0; h < height; h += 4) {
            for (int w = 0; w < width; ++w) {
                at(w, h) = Color(42, 42, 42);
            }
        }
        bool offset = false;
        for (int w = 0; w < width; w += 4) {
            int h = 0 + offset * 4;
            while (h < height) {
                at(w, h) = Color(42, 42, 42);
                if ((h + 1) % 4)
                    ++h;
                else
                    h += 5;
            }
            offset = !offset;
        }
    }
};
