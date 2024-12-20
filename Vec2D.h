#pragma once

#include <math.h>

struct Vec2D {
    double x;
    double y;

    Vec2D(double x_, double y_) : x(x_), y(y_) {}

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
