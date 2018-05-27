#pragma once

#include "Object.h"

namespace ObjectFactory {
    Object createPlane(std::size_t definition);
    Object createMountain(std::size_t definition);
    Object createLighthouse(std::size_t definition);
    Object createTurretBody(std::size_t definition);
    Object createTurretBarrel(std::size_t definition);
}
