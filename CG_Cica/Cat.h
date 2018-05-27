#pragma once

#include <glm/glm.hpp>

#include "Entity.h"

class Cat : public Entity {
public:
    void update(float dt) override;
    glm::mat4 getTransform() const override;

    void change_direction(glm::vec3 newDirection);
    void rotate_direction(float angle);

private:
    glm::vec3 direction = { 1, 0, 0 };

    float speed = 1.0f;
};
