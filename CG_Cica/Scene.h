#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Object.h"

class ProgramObject;

struct Node {
    virtual ~Node() = default;

    virtual void render(ProgramObject& shader, glm::mat4 vp, glm::mat4 m) {
        for (auto& child : children) {
            child->render(shader, vp, m);
        }
    }

    std::vector<std::shared_ptr<Node>> children;
};

struct Scene {
    void render(ProgramObject& shader, glm::mat4 projection, glm::mat4 view) {
        root->render(shader, projection * view, glm::mat4(1));
    }

    std::unique_ptr<Node> root;
};

struct TransformationNode : Node {
    void render(ProgramObject& shader, glm::mat4 vp, glm::mat4 m) override;

    glm::mat4 matrix;
};

struct ObjectNode : Node {
    void render(ProgramObject& shader, glm::mat4 vp, glm::mat4 m) override;

    Object object;
};

struct ShaderModeNode : Node {
    void render(ProgramObject& shader, glm::mat4 vp, glm::mat4 m) override;

    int mode;
};
