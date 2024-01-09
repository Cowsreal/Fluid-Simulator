#pragma once

#include "glm/glm.hpp"

#include "../../Renderer.hpp"
#include "../../VertexBuffer.hpp"
#include "../../VertexArray.hpp"
#include "../../VertexBufferLayout.hpp"


class Line
{
public:
    Line(glm::vec3 start, glm::vec3 end);

    void UpdateVertices(glm::vec3 start, glm::vec3 end);
    void Draw();

private:
    std::vector<float> m_vertices;
    VertexBuffer m_VBO;
    VertexArray m_VAO;
    VertexBufferLayout m_VBL;

};