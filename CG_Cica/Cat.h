#pragma once

#include <memory>
#include <random>

#include <glm/glm.hpp>

#include "Entity.h"

struct SDL_KeyboardEvent;
struct SDL_MouseMotionEvent;

class Turret;

class Cat : public Entity {
public:
    Cat();
    ~Cat();

    glm::mat4 getTransform() const override;

    void change_direction(glm::vec3 newDirection);
    void rotate_direction(float angle);

    const Turret& getTurret() const;

protected:
    glm::vec3 direction = { 1, 0, 0 };
    float speed = 0.0f;

    std::unique_ptr<Turret> turret;
};

class AICat : public Cat {
public:
    void update(float dt) override;

private:
    float time_until_turn = 5.0f;
    float turning_time = 0.0f;
    float turn_angle;

    static std::random_device rd;
    static std::mt19937 random_gen;
};

class PlayerCat : public Cat {
public:
    void update(float dt) override;

    void KeyboardDown(SDL_KeyboardEvent& key);
    void KeyboardUp(SDL_KeyboardEvent& key);
    void MouseMove(SDL_MouseMotionEvent& mouse);
};
