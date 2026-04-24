#pragma once
#include <cmath>
#include <string>

struct Vec2 {
    float x = 0.f, y = 0.f;

    Vec2() = default;
    Vec2(float x, float y) : x(x), y(y) {}

    Vec2 operator+(const Vec2& o) const { return { x + o.x, y + o.y }; }
    Vec2 operator-(const Vec2& o) const { return { x - o.x, y - o.y }; }
    Vec2 operator*(float s) const { return { x * s, y * s }; }
    Vec2 operator/(float s) const { return { x / s, y / s }; }
    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }
    Vec2& operator*=(float s) { x *= s; y *= s; return *this; }
    bool operator==(const Vec2& o) const { return x == o.x && y == o.y; }
    bool operator!=(const Vec2& o) const { return !(*this == o); }

    float length() const { return std::sqrt(x * x + y * y); }
    float lengthSq() const { return x * x + y * y; }
    Vec2 normalize() const { float l = length(); return l > 0 ? *this / l : Vec2{}; }
    float dot(const Vec2& o) const { return x * o.x + y * o.y; }
    float distance(const Vec2& o) const { return (*this - o).length(); }
    std::string toString() const { return "Vec2(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }
};

struct Vec3 {
    float x = 0.f, y = 0.f, z = 0.f;

    Vec3() = default;
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& o) const { return { x + o.x, y + o.y, z + o.z }; }
    Vec3 operator-(const Vec3& o) const { return { x - o.x, y - o.y, z - o.z }; }
    Vec3 operator*(float s) const { return { x * s, y * s, z * s }; }
    Vec3 operator/(float s) const { return { x / s, y / s, z / s }; }
    Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vec3& operator-=(const Vec3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    Vec3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
    bool operator==(const Vec3& o) const { return x == o.x && y == o.y && z == o.z; }
    bool operator!=(const Vec3& o) const { return !(*this == o); }

    float length() const { return std::sqrt(x * x + y * y + z * z); }
    float lengthSq() const { return x * x + y * y + z * z; }
    Vec3 normalize() const { float l = length(); return l > 0 ? *this / l : Vec3{}; }
    float dot(const Vec3& o) const { return x * o.x + y * o.y + z * o.z; }
    Vec3 cross(const Vec3& o) const { return { y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x }; }
    float distance(const Vec3& o) const { return (*this - o).length(); }
    float distanceXZ(const Vec3& o) const { return std::sqrt((x - o.x) * (x - o.x) + (z - o.z) * (z - o.z)); }
    Vec2 toVec2() const { return { x, z }; }
    std::string toString() const { return "Vec3(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")"; }
};

struct BlockPos {
    int x = 0, y = 0, z = 0;

    BlockPos() = default;
    BlockPos(int x, int y, int z) : x(x), y(y), z(z) {}
    explicit BlockPos(const Vec3& v) : x((int)v.x), y((int)v.y), z((int)v.z) {}

    BlockPos operator+(const BlockPos& o) const { return { x + o.x, y + o.y, z + o.z }; }
    BlockPos operator-(const BlockPos& o) const { return { x - o.x, y - o.y, z - o.z }; }
    BlockPos& operator+=(const BlockPos& o) { x += o.x; y += o.y; z += o.z; return *this; }
    BlockPos& operator-=(const BlockPos& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    bool operator==(const BlockPos& o) const { return x == o.x && y == o.y && z == o.z; }
    bool operator!=(const BlockPos& o) const { return !(*this == o); }

    float distance(const BlockPos& o) const {
        return std::sqrt((float)((x - o.x) * (x - o.x) + (y - o.y) * (y - o.y) + (z - o.z) * (z - o.z)));
    }
    Vec3 toVec3() const { return { (float)x, (float)y, (float)z }; }
    std::string toString() const { return "BlockPos(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")"; }
};