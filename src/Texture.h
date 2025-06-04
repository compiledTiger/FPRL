#pragma once

#include <cmath>
#include <cstdio>
#include <iostream>

#include "Color.h"

template <int width_, int height_>
struct Texture {
    static const int width = width_;
    static const int height = height_;
    static const int area = width * height;
    static const int darknessLevels = 64;
    static const int nPixels = darknessLevels * area;
    Color pixels[nPixels];

    // pixels() sets values to nullptr
    Texture() : pixels() {}

    Color& at(int x, int y, int darknessLevel = 0) {
        // TODO break out index?
        return pixels[darknessLevel * area + y * width + x];
    }

    void calcDarknessLevels() {
        for (int darknessLevel = 1; darknessLevel < darknessLevels;
             ++darknessLevel) {
            for (int i = 0; i < area; ++i) {
                pixels[darknessLevel * area + i] =
                    pixels[i] * (double(darknessLevels - darknessLevel) /
                                 double(darknessLevels));
            }
        }
    }

    void generateWall() {
        for (auto& p : pixels) p = Color(150, 150, 60);

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
        calcDarknessLevels();
    }
    void generateFloor(Color c) {
        for (auto& p : pixels) p = c;

        for (int h = 0; h < height; h += 8) {
            for (int w = 0; w < width; ++w) {
                at(w, h) = Color(42, 42, 42);
            }
        }
        bool offset = false;
        for (int w = 0; w < width; w += 4) {
            int h = 0 + offset * 8;
            while (h < height) {
                at(w, h) = Color(42, 42, 42);
                if ((h + 1) % 8)
                    ++h;
                else
                    h += 9;
            }
            offset = !offset;
        }
        calcDarknessLevels();
    }

    void loadBmp(const char* path) {
        FILE* f = fopen(path, "rb");
        unsigned char header[54];
        fread(header, sizeof(unsigned char), 54, f);
        int dibHeaderSize = *(int*)&header[14];
        int w = *(int*)&header[18];
        int h = *(int*)&header[22];
        int bitsPerPixel = *(int*)&header[28];
        int colorsUsed = *(int*)&header[46];
        // TODO assert same dimensions and 8 bit color mode

        std::cout << "Loading bmp file " << path << " with dimensions " << w
                  << "x" << h << ", " << colorsUsed << " colors used and "
                  << bitsPerPixel << " bits per pixel" << std::endl;

        fseek(f, dibHeaderSize - 40, SEEK_CUR);

        int bytesPerColor = 4;
        int sizeOfColorTable = colorsUsed * bytesPerColor;
        unsigned char* colorTable = new unsigned char[sizeOfColorTable];
        fread(colorTable, sizeof(unsigned char), sizeOfColorTable, f);

        if (colorsUsed == 16) {
            for (int paletteIndex1 = 0; paletteIndex1 < colorsUsed;
                 ++paletteIndex1) {
                Color color1(colorTable[paletteIndex1 * bytesPerColor + 2],
                             colorTable[paletteIndex1 * bytesPerColor + 1],
                             colorTable[paletteIndex1 * bytesPerColor]);
                std::cout << "WEE " << paletteIndex1 << " " << color1
                          << std::endl;
            }
        }

        Color transp(0, 0, 0, 0);

        int index = 0;
        unsigned char pixel[1];
        while (fread(pixel, sizeof(unsigned char), 1, f)) {
            if (colorsUsed == 16) {
                int paletteIndex1 = pixel[0] >> 4;
                int paletteIndex2 = (unsigned char)(pixel[0] << 4) >> 4;
                Color color1(colorTable[paletteIndex1 * bytesPerColor + 2],
                             colorTable[paletteIndex1 * bytesPerColor + 1],
                             colorTable[paletteIndex1 * bytesPerColor]);
                Color color2(colorTable[paletteIndex2 * bytesPerColor + 2],
                             colorTable[paletteIndex2 * bytesPerColor + 1],
                             colorTable[paletteIndex2 * bytesPerColor]);
                 std::cout << index << " " << paletteIndex1 << " " << color1
                 << " " << paletteIndex2 << " " << color2 << std::endl;
                if (color1 == Color(0, 0, 0)) color1 = transp;
                if (color2 == Color(0, 0, 0)) color2 = transp;
                pixels[area - index - 1] = color1;
                pixels[area - index - 2] = color2;
                // padding, 20 pixels stored in 10 bytes
                // row bytes must be a multiple of 4, 12 is closest
                index += 2;
                if ((index) % w == 0) {
                    std::cout << "!" << std::endl;
                    fseek(f, 2, SEEK_CUR);
                }
            } else {
                Color color = Color(colorTable[pixel[0] * bytesPerColor + 2],
                                    colorTable[pixel[0] * bytesPerColor + 1],
                                    colorTable[pixel[0] * bytesPerColor]);

                // std::cout << (int)color.r << " " << (int)color.g << " "
                //           << (int)color.b << " " << (int)color.a <<
                //           std::endl;

                // TODO only for rltiles
                if (color == Color(71, 108, 108))  // TODO put somewhere
                    color = transp;

                pixels[area - index - 1] = color;
                index++;
            }
        }

        for (int i = 0; i < area; ++i)
            std::cout << ":" << pixels[i] << std::endl;

        calcDarknessLevels();
    }
};
