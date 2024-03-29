#include "Circle.hpp"
#include "../../Renderer.hpp"

#include <iostream>

Circle::Circle()
{
}

Circle::Circle(std::vector<float> vertices, int numPoints, bool drawAsPoints = false, float pointSize = 1.0f)
    : m_Radius(500.0f), m_VAO(), m_VBO(), m_IBO(), m_VBL(), m_Vertices(vertices), m_Indices(), m_numCols(numPoints), m_DrawAsPoints(drawAsPoints), m_PointSize(pointSize)
{
    GenerateCircleIndices();
    m_IBO = IndexBuffer(&m_Indices[0], m_Indices.size());
    m_VBO = VertexBuffer(&m_Vertices[0], m_Vertices.size() * sizeof(float));
    m_VBL.Push<float>(3);
    m_VAO.AddBuffer(m_VBO, m_VBL, false);
}

Circle::Circle(float radius)
	: m_Radius(radius), m_VAO(), m_VBO(), m_IBO(), m_VBL(), m_Vertices(), m_Indices()
{
	GenerateCircleVertices();
	GenerateCircleIndices();
	m_IBO = IndexBuffer(&m_Indices[0], m_Indices.size());
	m_VBO = VertexBuffer(&m_Vertices[0], m_Vertices.size() * sizeof(float));
	m_VBL.Push<float>(3);
	m_VAO.AddBuffer(m_VBO, m_VBL, false);
}

Circle::Circle(const std::vector<Particle>* instanceData)
    : m_Radius(1.0f), m_VAO(), m_VBO(), m_IBO(), m_VBL(), m_Vertices(), m_Indices()
{
    m_IsInstanced = true;
    m_NumInstances = instanceData->size();
    GenerateCircleVertices();
    GenerateCircleIndices();
    m_IBO = IndexBuffer(&m_Indices[0], m_Indices.size());
    m_VBO = VertexBuffer(&m_Vertices[0], m_Vertices.size() * sizeof(float));

    m_VBL.Push<float>(3);
    m_VAO.Bind();
    m_VAO.AddBuffer(m_VBO, m_VBL, false);

    m_InstanceVBO = VertexBuffer(instanceData->data(), m_NumInstances * sizeof(Particle));
    m_InstanceVBO.Bind();

    GLsizei stride = sizeof(Particle);

    GLCall(glEnableVertexAttribArray(1));
    GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(Particle, Pos)));
    GLCall(glEnableVertexAttribArray(2));
    GLCall(glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(Particle, Color)));
    GLCall(glEnableVertexAttribArray(3));
    GLCall(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(Particle, Radius)));

    GLCall(glVertexAttribDivisor(1, 1));
    GLCall(glVertexAttribDivisor(2, 1));
    GLCall(glVertexAttribDivisor(3, 1));

    m_VAO.Unbind();
    m_InstanceVBO.Unbind();
}

void Circle::UpdateInstanceData(const std::vector<Particle>* instanceData)
{
    m_NumInstances = instanceData->size();
    m_InstanceVBO.UpdateData(instanceData->data(), m_NumInstances * sizeof(Particle));
}

Circle Circle::operator=(const Circle& other)
{
    if(this != &other)
    {
        m_IBO = other.m_IBO;
        m_VBL = other.m_VBL;
        m_VAO.Delete();
        m_VAO = other.m_VAO;
        m_VBO.Delete();
        m_VBO = other.m_VBO;
        m_Radius = other.m_Radius;
        m_Vertices = other.m_Vertices;
        m_Indices = other.m_Indices;
        m_numCols = other.m_numCols;
        m_DrawAsPoints = other.m_DrawAsPoints;
        m_PointSize = other.m_PointSize;
    }
    return *this;
}

void Circle::GenerateCircleVertices()
{
    m_Vertices.clear();
    float angleInc = 2 * PI / m_numCols;
    for(int i = 0; i <= m_numCols; i++)
    {
        float angle = angleInc * i;
        float x = m_Radius * cos(angle);
        float y = m_Radius * sin(angle);
        float z = 0.0f;
        m_Vertices.push_back(x);
        m_Vertices.push_back(y);
        m_Vertices.push_back(z);
    }
}

void Circle::GenerateCircleIndices()
{
    m_Indices.clear();
    for (int i = 0; i < m_numCols; i++)
    {
        m_Indices.push_back(i);
        m_Indices.push_back((i + 1) % m_numCols); // Wrap around to the first vertex
    }
}

void Circle::Draw()
{
	m_VAO.Bind();
	m_IBO.Bind();
    if(m_IsInstanced)
    {
        m_InstanceVBO.Bind();
        GLCall(glDrawElementsInstanced(GL_LINES, m_Indices.size(), GL_UNSIGNED_INT, nullptr, m_NumInstances));
    }
    else
    {
        if(m_DrawAsPoints)
        {
            GLCall(glPointSize(m_PointSize));
            GLCall(glDrawElements(GL_POINTS, m_Indices.size(), GL_UNSIGNED_INT, nullptr));
        }
        else
        {
            GLCall(glDrawElements(GL_LINES, m_Indices.size(), GL_UNSIGNED_INT, nullptr));
        }
    }
}

void Circle::ChangePointSize(float pointSize)
{
    m_PointSize = pointSize;
}

void Circle::SetDrawAsPoints(bool drawAsPoints)
{
    m_DrawAsPoints = drawAsPoints;
}
