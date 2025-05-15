#pragma once

#include <cmath>

#include "Color.h"

template <int width_, int height_>
struct Texture {
    static const int width = width_;
    static const int height = height_;
    static const int area = width * height;
    static const int darknessLevels = 64;
    static const int downsampleLevels = 2;
    static const int nPixels = downsampleLevels * darknessLevels * area;
    Color pixels[nPixels];

    // pixels() sets values to nullptr
    Texture() : pixels() {}

    Color& at(int x, int y, int darknessLevel = 0, int downsampleLevel = 0) {
        // TODO break out index?
        return pixels[downsampleLevel * darknessLevels * area +
                      darknessLevel * area + y * width + x];
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

    void calcDownsampleLevels() {
        for (int downsampleLevel = 1; downsampleLevel < downsampleLevels;
             ++downsampleLevel) {
            int blockSize = pow(2, downsampleLevel);
            for (int darknessLevel = 1; darknessLevel < darknessLevels;
                 ++darknessLevel) {
                for (int x = 0; x < width; x += blockSize) {
                    for (int y = 0; y < height; y += blockSize) {
                        Color mean =
                            at(x, y, darknessLevel, downsampleLevel - 1);
                        for (int xOfs = 0; xOfs < blockSize; ++xOfs) {
                            for (int yOfs = 0; yOfs < blockSize; ++yOfs) {
                                if (!xOfs && !yOfs) continue;
                                mean = mean.mean(at(x + xOfs, y + yOfs,
                                                    darknessLevel,
                                                    downsampleLevel - 1));
                            }
                        }
                        for (int xOfs = 0; xOfs < blockSize; ++xOfs) {
                            for (int yOfs = 0; yOfs < blockSize; ++yOfs) {
                                at(x + xOfs, y + yOfs, darknessLevel,
                                   downsampleLevel) = mean;
                            }
                        }
                    }
                }
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
        calcDownsampleLevels();
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
        calcDownsampleLevels();
    }

    void loadBmp(const char* path) {
        FILE* f = fopen(path, "rb");
        unsigned char header[54];
        fread(header, sizeof(unsigned char), 54, f);
        int w = *(int*)&header[18];
        int h = *(int*)&header[22];
        int bitsPerPixel = *(int*)&header[28];
        int colorsUsed = *(int*)&header[46];
        // TODO assert same dimensions and 8 bit color mode

        std::cout << "Loading bmp file " << path << " with dimensions " << w
                  << "x" << h << ", " << colorsUsed << " colors used and "
                  << bitsPerPixel << " bits per pixel" << std::endl;

        int bytesPerColor = 4;
        int sizeOfColorTable = colorsUsed * bytesPerColor;
        unsigned char* colorTable = new unsigned char[sizeOfColorTable];
        fread(colorTable, sizeof(unsigned char), sizeOfColorTable, f);

        int index = 0;
        unsigned char pixel[1];
        while (fread(pixel, sizeof(unsigned char), 1, f)) {
            pixels[area - index] =
                Color(colorTable[pixel[0] * bytesPerColor + 2],
                      colorTable[pixel[0] * bytesPerColor + 1],
                      colorTable[pixel[0] * bytesPerColor]);
            index++;
        }

        calcDarknessLevels();
        calcDownsampleLevels();
    }
};
