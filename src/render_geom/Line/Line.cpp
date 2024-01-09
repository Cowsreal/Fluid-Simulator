#include "Line.hpp"

Line::Line(glm::vec3 start, glm::vec3 end)
    : m_vertices(
        {
        start.x, start.y, start.z,
        end.x, end.y, end.z
        }), m_VBO(&m_vertices[0], m_vertices.size() * sizeof(float)), m_VAO(), m_VBL()
{
    m_VBL.Push<float>(3);
    m_VAO.AddBuffer(m_VBO, m_VBL, false);
}

void Line::UpdateVertices(glm::vec3 start, glm::vec3 end)
{
    m_vertices = {
        start.x, start.y, start.z,
        end.x, end.y, end.z
    };
    m_VBO.UpdateData(&m_vertices[0], m_vertices.size() * sizeof(float));
}

void Line::Draw()
{
    m_VAO.Bind();
    m_VBO.Bind();
    GLCall(glDrawArrays(GL_LINES, 0, 2));
}

