#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

struct Node {
    virtual ~Node() = default;

    virtual void render(glm::mat4 mvp) {
        for (auto& child : children) {
            child->render(mvp);
        }
    }

    std::vector<std::shared_ptr<Node>> children;
};

struct Scene {
    void render(glm::mat4 projection, glm::mat4 view) {
        root->render(projection * view * glm::mat4(1));
    }

    std::unique_ptr<Node> root;
};

struct TranslationNode : Node {
    void render(glm::mat4 mvp) override;

    glm::mat4 matrix;
};

struct ObjectNode : Node {
    void render(glm::mat4 mvp) override;
};
