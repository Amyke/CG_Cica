#pragma once

#include "Object.h"

namespace ObjectFactory {
    Object createPlane(std::size_t definition);
    Object createLighthouse(std::size_t definition);
}
