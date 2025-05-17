#pragma once

#include <math.h>

#include "FixedDir.h"

struct Vec2D {
    double x;
    double y;

    Vec2D(double x_, double y_) : x(x_), y(y_) {}

    Vec2D(FixedDir dir) {
        switch (dir.dir) {
            case FixedDir::Dir::n:
                x = 0;
                y = -1;
                break;
            case FixedDir::Dir::nw:
                x = -1;
                y = -1;
                break;
            case FixedDir::Dir::w:
                x = -1;
                y = 0;
                break;
            case FixedDir::Dir::sw:
                x = -1;
                y = 1;
                break;
            case FixedDir::Dir::s:
                x = 0;
                y = 1;
                break;
            case FixedDir::Dir::se:
                x = 1;
                y = 1;
                break;
            case FixedDir::Dir::e:
                x = 1;
                y = 0;
                break;
            case FixedDir::Dir::ne:
                x = 1;
                y = -1;
                break;
        }
    }

    Vec2D operator+(const Vec2D& rhs) const {
        return Vec2D(x + rhs.x, y + rhs.y);
    }
    Vec2D operator-(const Vec2D& rhs) const {
        return Vec2D(x - rhs.x, y - rhs.y);
    }
    Vec2D operator*(const double& f) const { return Vec2D(x * f, y * f); }
    Vec2D operator/(const double& f) const { return Vec2D(x / f, y / f); }
    Vec2D norm() const { return *this / mag(); }
    double mag() const { return std::sqrt(x * x + y * y); };
    double dot(const Vec2D& rhs) const { return x * rhs.x + y * rhs.y; }
    double det(const Vec2D& rhs) const { return x * rhs.y - y * rhs.x; }

    Vec2D& operator+=(const Vec2D& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    Vec2D& operator-=(const Vec2D& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }
    Vec2D& operator*=(const double& f) {
        x *= f;
        y *= f;
        return *this;
    }
    Vec2D& operator/=(const double& f) {
        x /= f;
        y /= f;
        return *this;
    }
};
