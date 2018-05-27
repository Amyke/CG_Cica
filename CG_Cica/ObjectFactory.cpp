#include "ObjectFactory.h"

#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>

#include <SDL.h>

#include "Noise.h"

struct Vertex {
    glm::vec3 p;
    glm::vec3 c;
    glm::vec2 texCoord;
};

namespace plane {
    //négyzet pontok kirajzolása
    std::vector<Vertex> vertices(std::size_t definition) {
        std::vector<Vertex> vertices;
        vertices.reserve(std::pow(definition + 1, 2));
        glm::vec3 color{ 0.0, 0.5, 0.8 };
        double definitionD = definition;
        for (std::size_t z = 0; z <= definition; ++z)
        {
            float texU = (z % 25) / 24.0;
            for (std::size_t x = 0; x <= definition; ++x)
            {
                float texV = (x % 25) / 24.0;
                vertices.push_back({ glm::vec3{ -1.0 + 2.0 * x / definitionD, 0.0, -1.0 + 2.0 * z / definitionD }, color, glm::vec2{ texU, texV } });
            }
        }
        return vertices;
    }

    //a csúcsok összekötése háromszögek által
    std::vector<GLushort> indices(std::size_t definition)
    {
        std::vector<GLushort> indices;
        indices.reserve(std::pow(definition + 1, 2) * 6);
        for (std::size_t z = 0; z < definition; ++z)
        {
            for (std::size_t x = 1; x <= definition; ++x)
            {
                GLushort me = z * (definition + 1) + x;
                GLushort nextCol = (z + 1) * (definition + 1) + x;

                indices.push_back(me);
                indices.push_back(me - 1);
                indices.push_back(nextCol - 1);

                indices.push_back(me);
                indices.push_back(nextCol - 1);
                indices.push_back(nextCol);
            }
        }
        return indices;
    }
}

double inverted_lerp(double min, double max, double t) {
    return (t - min) / (max - min);
}

namespace mountain {
    std::vector<Vertex> vertices(std::size_t definition) {
        std::vector<Vertex> vertices;
        vertices.reserve(std::pow(definition + 1, 2));
        glm::vec3 color{ 0.0, 0.5, 0.8 };
        double definitionD = definition;

        const Perlin perlin;
        double min = std::numeric_limits<double>::max();
        double max = std::numeric_limits<double>::min();
        for (std::size_t z = 0; z <= definition; ++z) {
            float texU = (z % 25) / 24.0;
            for (std::size_t x = 0; x <= definition; ++x) {
                float texV = (x % 25) / 24.0;
                double xi = 2 * x / definitionD - 1;
                double zi = 2 * z / definitionD - 1;
                double length = xi * xi + zi * zi;
                double y = 2 * length - 1;
                double noise = perlin.octave_noise(4, x * 20.0 / definitionD, y * 20.0 / definitionD) + 1;
                y += 0.25 * noise;
                auto p = glm::vec3{ xi, y, zi };
                vertices.push_back({ p, color, glm::vec2{ texU, texV } });
            }
        }
        return vertices;
    }
}

namespace ObjectFactory {
    Object createPlane(std::size_t definition) {
        Object ret;

        auto vertices = plane::vertices(definition);
        auto indices = plane::indices(definition);

        ArrayBuffer vbo;
        ret.vbo.BufferData(vertices);
        ret.ibo.BufferData(indices);

        ret.vao.Init({
            { CreateAttribute<0, glm::vec3, 0, sizeof Vertex>, ret.vbo },
            { CreateAttribute<1, glm::vec3, sizeof glm::vec3, sizeof Vertex>, ret.vbo },
            { CreateAttribute<2, glm::vec2, 2 * sizeof glm::vec3, sizeof Vertex>, ret.vbo }
        }, ret.ibo);

        return ret;
    }

    Object createMountain(std::size_t definition) {
        Object ret;

        auto vertices = mountain::vertices(definition);
        glm::vec3 white(1, 1, 1);
        glm::vec3 green(52.0 / 256, 150.0 / 256, 62.0 / 256);
        const Perlin perlin;
        for (auto& vert : vertices) {
            if (std::abs(vert.p.x * 2) < 1.0 && std::abs(vert.p.z * 2) < 1.0) {
                vert.p.y = -1;
            } /*else {
                auto x = vert.p.x * .5 + .5;
                auto z = vert.p.z * .5 + .5;
                vert.p.y += 1.5 * perlin.octave_noise(8, x * 0.2, z * 0.2);
            }*/
            vert.c = glm::lerp(green, white, vert.p.y - 1);
        }
        auto indices = plane::indices(definition);

        ArrayBuffer vbo;
        ret.vbo.BufferData(vertices);
        ret.ibo.BufferData(indices);

        ret.vao.Init({
            { CreateAttribute<0, glm::vec3, 0, sizeof Vertex>, ret.vbo },
            { CreateAttribute<1, glm::vec3, sizeof glm::vec3, sizeof Vertex>, ret.vbo },
            { CreateAttribute<2, glm::vec2, 2 * sizeof glm::vec3, sizeof Vertex>, ret.vbo }
        }, ret.ibo);

        return ret;
    }
}

namespace lighthouse {
    //henger kirajzolása
    std::vector<Vertex> vertices(std::size_t definition) {
        const double radius = 0.5; //kör sugár

        std::vector<Vertex> vertices;

        const glm::vec3 bottom(0, 0, 0);
        vertices.push_back(Vertex{ bottom, bottom });

        for (std::size_t i = 0; i < definition; ++i) {
            //körgyûrû egy pontja
            Vertex v;
            double u = i / static_cast<double>(definition); //vetítés 0-1 közé
            u *= 2 * M_PI; //teljes kör rajzolása
                           // alja
            v.p = glm::vec3(radius * std::cos(u), 0, radius * std::sin(u));
            v.c = glm::normalize(v.p);
            vertices.push_back(v);
            // teteje
            v.p += glm::vec3(0, 4, 0);
            v.c = glm::normalize(v.p);
            vertices.push_back(v);
        }

        const glm::vec3 top(0, 6, 0);
        vertices.push_back(Vertex{ top, top });

        return vertices;
    }


    std::vector<GLushort> indices(std::size_t definition) {
        const std::size_t definition2 = definition * 2;
        std::vector<GLushort> indices;

        GLushort origin = 0; //körlap középpont
        GLushort top = definition2 + 1; //kúp csúcsa

        for (GLushort j = origin; j < definition2; j += 2)
        {
            //körlapon lévõ háromszövek
            indices.push_back((j + 3) % definition2);
            indices.push_back(origin);
            indices.push_back(j + 1);


            //fölsõ csúccsal való összekötés
            indices.push_back(j + 2);
            indices.push_back(top);
            indices.push_back(((j + 2) % definition2) + 2);

            //körlapok összekötése
            indices.push_back((j + 3) % definition2);
            indices.push_back(j + 1);
            indices.push_back(j + 2);

            indices.push_back((j + 3) % definition2);
            indices.push_back(j + 2);
            indices.push_back(((j + 2) % definition2) + 2);
        }

        return indices;
    }
}

namespace ObjectFactory {
    Object createLighthouse(std::size_t definition) {
        Object ret;

        auto vertices = lighthouse::vertices(definition);
        auto indices = lighthouse::indices(definition);

        ArrayBuffer vbo;
        ret.vbo.BufferData(vertices);
        ret.ibo.BufferData(indices);

        ret.vao.Init({
            { CreateAttribute<0, glm::vec3, 0, sizeof Vertex>, ret.vbo },
            { CreateAttribute<1, glm::vec3, sizeof glm::vec3, sizeof Vertex>, ret.vbo },
            { CreateAttribute<2, glm::vec2, 2 * sizeof glm::vec3, sizeof Vertex>, ret.vbo }
        }, ret.ibo);

        return ret;
    }
}
