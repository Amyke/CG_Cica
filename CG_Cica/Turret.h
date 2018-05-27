#pragma once

#include"Entity.h"

class Turret : public Entity {
public:
    void update(float dt) override;
    glm::mat4 getTransform() const override;

    void rotate(float angle);
    void tilt(float angle);

private:
    float angleY = 0.0f;
    float angleZ = 0.0f;
};
