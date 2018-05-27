#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform2.hpp>

struct Entity {
    virtual ~Entity() = default;

    virtual void update(float dt) = 0;

    virtual glm::mat4 getTransform() const {
        return glm::translate(position);
    }

    glm::vec3 position = { 0, 0, 0 };
};
