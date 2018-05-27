#include "Turret.h"

#include <cmath>

#include <SDL.h>

#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Turret::update(float dt) {
}

glm::mat4 Turret::getTransform() const {
    return
        glm::rotate(angleZ, glm::vec3(0, 0, 1)) *
        glm::rotate(angleY, glm::vec3(0, 1, 0));
}

void Turret::rotate(float angle) {
    angleY = std::fmod(angleY + angle, 2 * M_PI);
}

void Turret::tilt(float angle) {
    angleZ = glm::clamp<float>(angleZ + angle, (-20.0 / 360) * 2 * M_PI, (60.0 / 360) * 2 * M_PI);
}
