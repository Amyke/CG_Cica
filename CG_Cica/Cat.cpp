#include "Cat.h"

#include <cmath>

#include <SDL.h>

#include <glm/gtx/rotate_vector.hpp>

void Cat::update(float dt) {
    position += direction * speed * dt;
    position = glm::clamp(position, glm::vec3(-20, 0, -20), glm::vec3(20, 0, 20));
}

glm::mat4 Cat::getTransform() const {
    glm::vec3 b = glm::vec3(1, 0, 0);
    glm::vec3 v = glm::cross(b, direction) + glm::vec3(0, 0.001, 0);
    float angle = acos(glm::dot(b, direction));
    glm::mat4 rotation = glm::rotate(angle, v);
    return Entity::getTransform() * rotation;
}

void Cat::change_direction(glm::vec3 newDirection) {
    direction = glm::normalize(newDirection);
}

void Cat::rotate_direction(float angle) {
    direction = glm::normalize(glm::rotateY(direction, angle));
}
