#include "Noise.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

std::array<std::int32_t, 512> generate_p() {
    std::array<std::int32_t, 512> ret;
    auto mid = ret.begin() + 256;

    for (std::size_t i = 0; i < 256; ++i) {
        ret[i] = i;
    }

    std::random_device rd;
    std::mt19937 gen(rd());

    std::shuffle(ret.begin(), mid, gen);

    std::copy(ret.begin(), mid, mid);
    return ret;
}

double fade(double t) {
    // 6 t^5 - 15 t^4 + 10 t^3
    const double t3 = t * t * t;
    return t3 * (10 + t * (6 * t - 15));
}

double lerp(double t, double a, double b) {
    return a + t * (b - a);
}

double grad(std::int32_t hash, double x, double y, double z) {
    switch (hash & 15) {
        case 0: return  x + y;
        case 1: return -x + y;
        case 2: return  x - y;
        case 3: return -x - y;
        case 4: return  x + z;
        case 5: return -x + z;
        case 6: return  x - z;
        case 7: return -x - z;
        case 8: return  y + z;
        case 9: return -y + z;
        case 10: return  y - z;
        case 11: return -y - z;
        case 12: return  y + x;
        case 13: return -y + z;
        case 14: return  y - x;
        case 15: return -y - z;
        default: return 0; // never happens
    }
}

Perlin::Perlin()
    : p(generate_p()) {
}

double Perlin::noise(double x, double y, double z) {
    double xfloor = std::floor(x);
    double yfloor = std::floor(y);
    double zfloor = std::floor(z);

    std::int32_t xi = static_cast<std::int32_t>(xfloor) & 255;
    std::int32_t yi = static_cast<std::int32_t>(yfloor) & 255;
    std::int32_t zi = static_cast<std::int32_t>(zfloor) & 255;

    double xf = x - xfloor;
    double yf = y - yfloor;
    double zf = z - zfloor;

    double u = fade(xf);
    double v = fade(yf);
    double w = fade(zf);

    std::int32_t A = p[xi] + yi;
    std::int32_t AA = p[A] + zi;
    std::int32_t AB = p[A + 1] + zi;

    std::int32_t B = p[xi + 1] + yi;
    std::int32_t BA = p[B] + zi;
    std::int32_t BB = p[B + 1] + zi;

    double g1 = grad(    p[AA],     xf,     yf,     zf);
    double g2 = grad(    p[BA], xf - 1,     yf,     zf);
    double g3 = grad(    p[AB],     xf, yf - 1,     zf);
    double g4 = grad(    p[BB], xf - 1, yf - 1,     zf);
    double g5 = grad(p[AA + 1],     xf,     yf, zf - 1);
    double g6 = grad(p[BA + 1], xf - 1,     yf, zf - 1);
    double g7 = grad(p[AB + 1],     xf, yf - 1, zf - 1);
    double g8 = grad(p[BB + 1], xf - 1, yf - 1, zf - 1);

    double L_11 = lerp(u, g1, g2);
    double L_12 = lerp(u, g3, g4);

    double L_21 = lerp(u, g5, g6);
    double L_22 = lerp(u, g7, g8);

    double L_1 = lerp(v, L_11, L_12);
    double L_2 = lerp(v, L_21, L_22);

    return lerp(w, L_1, L_2);
}
