#pragma once

#include <array>
#include <cstdint>

class Perlin {
public:
    explicit Perlin();

    double noise(double x, double y = 0.0, double z = 0.0);

private:
    const std::array<std::int32_t, 512> p;
};
