#pragma once

// NOTICE: Don't remove, 'GL utilities' implicitly depends on this header
#include <glm/glm.hpp>

#include "BufferObject.h"
#include "VertexArrayObject.h"

struct Object {
    VertexArrayObject	vao;      // VAO objektum
    IndexBuffer			ibo;  // indexek
    ArrayBuffer         vbo;
};
