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

    void add_child(const std::shared_ptr<Node>& child) {
        children.emplace_back(child);
    }

    void add_child(std::shared_ptr<Node>&& child) {
        children.emplace_back(child);
    }

private:
    std::vector<std::shared_ptr<Node>> children;
};

struct Scene {
    Scene(std::unique_ptr<Node> root_) : root(std::move(root_)) {
    }

    void render(ProgramObject& shader, glm::mat4 projection, glm::mat4 view) {
        root->render(shader, projection * view, glm::mat4(1));
    }

    std::unique_ptr<Node> root;
};

struct TransformationNode : Node {
    TransformationNode(glm::mat4 matrix_) : matrix(std::move(matrix_)) {
    }

    void render(ProgramObject& shader, glm::mat4 vp, glm::mat4 m) override;

    glm::mat4 matrix;
};

struct ObjectNode : Node {
    ObjectNode(Object object_) : object(std::move(object_)) {
    }

    void render(ProgramObject& shader, glm::mat4 vp, glm::mat4 m) override;

    Object object;
};

enum class ShaderMode {
    Normal = 0,
    Water = 1
};

struct ShaderModeNode : Node {
    ShaderModeNode(ShaderMode mode_) : mode(mode_) {
    }

    void render(ProgramObject& shader, glm::mat4 vp, glm::mat4 m) override;

    ShaderMode mode;
};
