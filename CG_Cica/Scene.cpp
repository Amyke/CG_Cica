#include "Scene.h"

void TranslationNode::render(glm::mat4 mvp) {
    glm::mat4 modified = mvp * matrix;
    Node::render(modified);
}

void ObjectNode::render(glm::mat4 mvp) {

}
