#include "Cat.h"

#include <algorithm>
#include <cmath>

#include <SDL.h>

#include <glm/gtx/rotate_vector.hpp>

#include "Turret.h"

glm::vec3 clamp_to_world(glm::vec3 p) {
    return glm::clamp(p, glm::vec3(-18.75, 0, -18.75), glm::vec3(18.75, 0, 18.75));
}

Cat::Cat()
    : turret(std::make_unique<Turret>()) {
}

Cat::~Cat() = default;

void Cat::update(float dt) {
    speed = std::min(std::max(speed + 0.01f, -1.0f), 1.0f);
    position += direction * speed * dt;
    position = clamp_to_world(position);
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
    float new_speed = std::abs(speed) - 0.05f;
    if (new_speed > 0.01f) {
        speed = std::copysign(new_speed, speed);
    }
    direction = glm::normalize(glm::rotateY(direction, angle));
}

const Turret& Cat::getTurret() const {
    return *turret;
}

void PlayerCat::update(float dt) {
    position += direction * speed * dt;
    position = clamp_to_world(position);
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
    case SDLK_q:
        turret->rotate(0.05f);
        break;
    case SDLK_e:
        turret->rotate(-0.05f);
        break;
    case SDLK_2:
        turret->tilt(-0.05);
        break;
    case SDLK_3:
        turret->tilt(0.05);
        break;
    }
    speed = std::min(std::max(speed, -1.0f), 1.0f);
}

void PlayerCat::KeyboardUp(SDL_KeyboardEvent & key) {
}

void PlayerCat::MouseMove(SDL_MouseMotionEvent & mouse) {
}
