#pragma once

#include <memory>

// NOTICE: Don't remove, 'GL utilities' implicitly depends on this header
#include <glm/glm.hpp>

#include "BufferObject.h"
#include "VertexArrayObject.h"
#include "TextureObject.h"

struct Vertex {
    glm::vec3 p;
    glm::vec3 c;
    glm::vec2 texCoord;
    glm::vec3 normal;
};

struct Object {
    VertexArrayObject	vao;      // VAO objektum
    IndexBuffer			ibo;  // indexek
    ArrayBuffer         vbo;

    std::unique_ptr<TextureObject<>> texture;
    std::unique_ptr<TextureObject<>> normal_map;
};
