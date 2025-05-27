#pragma once

struct FixedDir {
    enum class Dir { n, nw, w, sw, s, se, e, ne };
    Dir dir = Dir::n;

    FixedDir() = default;
    FixedDir(Dir dir_) : dir(dir_){};

    FixedDir operator+(int n) {
        FixedDir copy = *this;
        for (int i = 0; i < n; ++i) ++copy;
        return copy;
    }
    FixedDir operator-(int n) {
        FixedDir copy = *this;
        for (int i = 0; i < n; ++i) --copy;
        return copy;
    }
    int operator-(const FixedDir& rhs) {
        FixedDir copy = *this;
        int i = 0;
        for (; copy.dir != rhs.dir; ++i) ++copy;
        return i;
    }
    bool operator==(const FixedDir& rhs) const { return dir == rhs.dir; }
    bool operator!=(const FixedDir& rhs) const { return !(*this == rhs); }

    // TODO align rotations globally
    // Counter clockwise
    void operator++() {
        switch (dir) {
            case Dir::n:
                dir = Dir::nw;
                break;
            case Dir::nw:
                dir = Dir::w;
                break;
            case Dir::w:
                dir = Dir::sw;
                break;
            case Dir::sw:
                dir = Dir::s;
                break;
            case Dir::s:
                dir = Dir::se;
                break;
            case Dir::se:
                dir = Dir::e;
                break;
            case Dir::e:
                dir = Dir::ne;
                break;
            case Dir::ne:
                dir = Dir::n;
                break;
        };
    }
    // Clockwise
    void operator--() {
        switch (dir) {
            case Dir::n:
                dir = Dir::ne;
                break;
            case Dir::nw:
                dir = Dir::n;
                break;
            case Dir::w:
                dir = Dir::nw;
                break;
            case Dir::sw:
                dir = Dir::w;
                break;
            case Dir::s:
                dir = Dir::sw;
                break;
            case Dir::se:
                dir = Dir::s;
                break;
            case Dir::e:
                dir = Dir::se;
                break;
            case Dir::ne:
                dir = Dir::e;
                break;
        };
    }
};
