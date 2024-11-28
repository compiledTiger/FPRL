#pragma once

struct Vec2D {
    double x;
    double y;

    Vec2D(double x_, double y_) : x(x_), y(y_) {}

    Vec2D operator+(const Vec2D& rhs) { return Vec2D(x + rhs.x, y + rhs.y); }
    Vec2D operator-(const Vec2D& rhs) { return Vec2D(x - rhs.x, y - rhs.y); }
    Vec2D operator*(const double& f) { return Vec2D(x * f, y * f); }
    double dot(const Vec2D& rhs) { return x * rhs.x + y * rhs.y; }

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
};
