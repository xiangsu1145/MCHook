#pragma once
#include <cmath>
#include <cstdlib> // 引入 rand()

class MathUtils {
public:
    static float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }

    static float clamp(float val, float minVal, float maxVal) {
        if (val < minVal) return minVal;
        if (val > maxVal) return maxVal;
        return val;
    }



    static float animateTo(float current, float target, float speed, float dt) {
        float diff = target - current;
        float step = diff * speed * dt;

        // 修复 3: 删除了 return target 后面的分号 ";"
        if (std::abs(step) > std::abs(diff)) {
            return target;
        }
        return current + step;
    }

    static int random(int min, int max) {
        return min + rand() % (max - min + 1);
    }

    static float toRadians(float deg) { return deg * 3.14159265f / 180.0f; }
    static float toDegrees(float rad) { return rad * 180.0f / 3.14159265f; }
};