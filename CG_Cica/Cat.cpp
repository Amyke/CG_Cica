#include "Cat.h"

#include <algorithm>
#include <cmath>

#include <SDL.h>

#include <glm/gtx/rotate_vector.hpp>

void Cat::update(float dt) {
    speed = std::min(std::max(speed + 0.01f, -1.0f), 1.0f);
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

void PlayerCat::update(float dt) {
    position += direction * speed * dt;
    position = glm::clamp(position, glm::vec3(-20, 0, -20), glm::vec3(20, 0, 20));
}

void PlayerCat::KeyboardDown(SDL_KeyboardEvent& key) {
    switch (key.keysym.sym) {
    case SDLK_w:
        speed += 0.1f;
        break;
    case SDLK_s:
        speed -= 0.1f;
        break;
    case SDLK_a:
        rotate_direction(0.05f);
        break;
    case SDLK_d:
        rotate_direction(-0.05f);
        break;
    }
    speed = std::min(std::max(speed, -1.0f), 1.0f);
}

void PlayerCat::KeyboardUp(SDL_KeyboardEvent & key) {
}

void PlayerCat::MouseMove(SDL_MouseMotionEvent & mouse) {
}
