#include "Scene.h"

#include <glm/gtx/transform2.hpp>

#include "ProgramObject.h"

#include "Entity.h"

void TransformationNode::render(ProgramObject& shader, glm::mat4 vp, glm::mat4 m) {
    glm::mat4 modified = m * matrix;
    Node::render(shader, vp, modified);
}

void ObjectNode::render(ProgramObject& shader, glm::mat4 vp, glm::mat4 m) {
    object.vao.Bind();

    shader.SetUniform("MVP", vp * m);
    shader.SetUniform("worldIT", glm::transpose(glm::inverse(m)));

    if (object.texture != nullptr) {
        shader.SetUniform("use_texture", GL_TRUE);
        shader.SetTexture("texture_", 0, *object.texture);
    } else {
        shader.SetUniform("use_texture", GL_FALSE);
    }
    if (object.normal_map != nullptr) {
        shader.SetUniform("use_normal", GL_TRUE);
        shader.SetTexture("normalMap", 1, *object.normal_map);
    } else {
        shader.SetUniform("use_normal", GL_FALSE);
    }

    glDrawElements(GL_TRIANGLES, object.ibo.sizeInBytes() / sizeof GLushort, GL_UNSIGNED_SHORT, nullptr);

    object.vao.Unbind();
}

void ShaderModeNode::render(ProgramObject& shader, glm::mat4 vp, glm::mat4 m) {
    shader.SetUniform("mode", static_cast<int>(mode));
    Node::render(shader, vp, m);
}

void EntityNode::render(ProgramObject& shader, glm::mat4 vp, glm::mat4 m) {
    glm::mat4 modified = m * entity.getTransform();
    Node::render(shader, vp, modified);
}
