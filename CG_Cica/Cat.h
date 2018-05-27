#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "Entity.h"

struct SDL_KeyboardEvent;
struct SDL_MouseMotionEvent;

class Turret;

class Cat : public Entity {
public:
    Cat();
    ~Cat();

    void update(float dt) override;
    glm::mat4 getTransform() const override;

    void change_direction(glm::vec3 newDirection);
    void rotate_direction(float angle);

    const Turret& getTurret() const;

protected:
    glm::vec3 direction = { 1, 0, 0 };
    float speed = 0.0f;

    std::unique_ptr<Turret> turret;
};

class PlayerCat : public Cat {
public:
    void update(float dt) override;

    void KeyboardDown(SDL_KeyboardEvent& key);
    void KeyboardUp(SDL_KeyboardEvent& key);
    void MouseMove(SDL_MouseMotionEvent& mouse);
};
