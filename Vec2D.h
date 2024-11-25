#pragma once

struct Vec2D {
    float x;
    float y;

    Vec2D(float x_, float y_) : x(x_), y(y_) {}

    Vec2D operator+(const Vec2D& rhs) { return Vec2D(x + rhs.x, y + rhs.y); }
    Vec2D operator-(const Vec2D& rhs) { return Vec2D(x - rhs.x, y - rhs.y); }
    Vec2D operator*(const float & f) { return Vec2D(x*f, y*f); }
    Vec2D & operator+=(const Vec2D& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    Vec2D & operator-=(const Vec2D& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }
    Vec2D & operator*=(const float & f) {
        x *= f;
        y *= f;
        return *this;
    }
    float dot(const Vec2D& rhs) { return x * rhs.x + y * rhs.y; }
};
