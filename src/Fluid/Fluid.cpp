#include "Fluid.hpp"
#include <iostream>
#include <cmath>

float Poly6(float r, float h)
{
    float volume = PI * pow(h, 8) / 4.0f;
    float value = std::max(0.0f, h * h - r * r);
    return pow(value, 3) / volume;
}

float gradPoly6(float r, float h)
{
    if (r > h)
    {
        return 0.0f;
    }
    else
    {
        float coeff = -945.0f / (32.0f * PI * pow(h, 9));
        float term = pow(h * h - r * r, 2) * r;
        return coeff * term;
    }
}

size_t Hash(int x, int y, int numCells)
{
    size_t p1 = 1572869;
    size_t p2 = 3145739;
    return ((x * p1) + (y * p2)) % numCells;
}

Fluid::Fluid(unsigned int numParticles, unsigned int initCols)
{
    m_NumParticles = numParticles;
    m_InitCols = initCols;

    m_Particles = std::vector<Particle>(m_NumParticles);

    float spacing = 10.0f;
    float x = - spacing * (m_InitCols / 2);
    float y = 0.0f;
    float z = 0.0f;

    for (unsigned int i = 0; i < m_NumParticles; i++)
    {
        if (i % m_InitCols == 0)
        {
            x = - spacing * (m_InitCols / 2);
            y += spacing;
        }
        m_Particles[i].Pos = glm::vec3(x, y, z);
        m_Particles[i].Vel = glm::vec3(0.0f, 0.0f, 0.0f);
        m_Particles[i].Acc = glm::vec3(0.0f, 0.0f, 0.0f);
        m_Particles[i].Color = glm::vec3(0.0f, 0.0f, 0.0f);
        m_Particles[i].Density = 0.0f;
        m_Particles[i].Pressure = 0.0f;
        m_Particles[i].HashVal = 0;
        m_Particles[i].Radius = 5.0f;

        x += spacing;
    }

    m_minBoundary = glm::vec2(-500.0f, -500.0f);
    m_maxBoundary = glm::vec2(500.0f, 500.0f);
}

void Fluid::Update()
{
    // Update positions
    for(unsigned int i = 0; i < m_NumParticles; i++)
    {
        m_Particles[i].Pos += m_Particles[i].Vel * m_Dt;
        m_Particles[i].Vel += m_Particles[i].Acc * m_Dt;
        if(m_Particles[i].Pos.x < m_minBoundary.x)
        {
            m_Particles[i].Pos.x = m_minBoundary.x;
            m_Particles[i].Vel.x *= -1 * 0.2;
        }
        if(m_Particles[i].Pos.x > m_maxBoundary.x)
        {
            m_Particles[i].Pos.x = m_maxBoundary.x;
            m_Particles[i].Vel.x *= -1 * 0.2;
        }
        if(m_Particles[i].Pos.y < m_minBoundary.y)
        {
            m_Particles[i].Pos.y = m_minBoundary.y;
            m_Particles[i].Vel.y *= -1 * 0.2;
        }
        if(m_Particles[i].Pos.y > m_maxBoundary.y)
        {
            m_Particles[i].Pos.y = m_maxBoundary.y;
            m_Particles[i].Vel.y *= -1 * 0.2;
        }
    }
    UpdateHashTable();
    CalculateDensity();
    UpdateAcceleration();
    m_Circle->UpdateInstanceData(&m_Particles);
}

void Fluid::UpdateAcceleration()
{
    // Calculate acceleration vector for each particle
    for(int i = 0; i < m_NumParticles; i++)
    {
        glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);

        // Neighborhood search

        // Loop over 3x3 grid of cells around particle

        int X0 = floor(m_Particles[i].Pos.x / m_SmoothingRadius);
        int Y0 = floor(m_Particles[i].Pos.y / m_SmoothingRadius);

        float density = m_Particles[i].Density;
        float pressure = density * m_K;

        for(int j = 0; j < 9; j++)
        {
            // Get current cell offset

            int X = X0 + j % 3 - 1;
            int Y = Y0 + j / 3 - 1;

            // Get hash value of current cell

            size_t hashVal = Hash(X, Y, ((m_maxBoundary.x - m_minBoundary.x) / m_SmoothingRadius) * ((m_maxBoundary.y - m_minBoundary.y) / m_SmoothingRadius));

            // If the cell exists, loop over particles in the cell

            if(m_HashTable.find(hashVal) != m_HashTable.end())
            {

                // Calculate force on particle

                for(unsigned int k = 0; k < m_HashTable[hashVal].size(); k++)
                {
                    // Pressure
                    float currDensity = m_HashTable[hashVal][k]->Density;
                    float currPressure = currDensity * m_K;
                    float r = glm::length(m_Particles[i].Pos - m_HashTable[hashVal][k]->Pos);
                    
                    if(r > 0.0f)
                    {
                        // Direction vector from particle to neighbor
                        glm::vec3 dir = glm::normalize(m_Particles[i].Pos - m_HashTable[hashVal][k]->Pos);
                        force += -1.0f * m_Mass * ((pressure + currPressure) / (2 * currDensity)) * gradPoly6(r, m_SmoothingRadius) * dir;
                    }
                    // Viscosity
                }
            }
        }

        // Gravity
        force += -1.0f * m_Mass * glm::vec3(0.0f, 300.0f, 0.0f);

        // Update acceleration

        m_Particles[i].Acc = force / m_Mass;
    }
}

void Fluid::CalculateDensity()
{
    for(unsigned int i = 0; i < m_NumParticles; i++)
    {
        int X0 = (int)((m_Particles[i].Pos.x - m_minBoundary.x) / m_SmoothingRadius);
        int Y0 = (int)((m_Particles[i].Pos.y - m_minBoundary.y) / m_SmoothingRadius);
        float density = 0.0f;

        // Loop over 3x3 grid of cells around particle
        for(int j = 0; j < 9; j++)
        {
            // Get current cell offset
            int X = X0 + j % 3 - 1;
            int Y = Y0 + j / 3 - 1;
            // Get hash value of current cell
            size_t hashVal = Hash(X, Y, ((m_maxBoundary.x - m_minBoundary.x) / m_SmoothingRadius) * ((m_maxBoundary.y - m_minBoundary.y) / m_SmoothingRadius));

            // If the cell exists, loop over particles in the cell
            if(m_HashTable.find(hashVal) != m_HashTable.end())
            {
                for(unsigned int k = 0; k < m_HashTable[hashVal].size(); k++)
                {
                    float r = glm::length(m_Particles[i].Pos - m_HashTable[hashVal][k]->Pos);
                    density += m_Mass * Poly6(r, m_SmoothingRadius);
                }
            }
        }
        m_Particles[i].Density = density;
        std::cout << density << std::endl;
    }
}

void Fluid::UpdateHashTable()
{    
    float cellSize = m_SmoothingRadius * 0.7f;

    int numCells = floor((m_maxBoundary.x - m_minBoundary.x) / cellSize) * floor((m_maxBoundary.y - m_minBoundary.y) / cellSize);
    m_HashTable.clear();
    for(unsigned int i = 0; i < m_NumParticles; i++)
    {
        glm::vec3 relPos = m_Particles[i].Pos - glm::vec3(m_minBoundary.x, m_minBoundary.y, 0.0f);
        int X = floor(relPos.x / m_SmoothingRadius);
        int Y = floor(relPos.y / m_SmoothingRadius);
        size_t hashVal = Hash(X, Y, numCells);
        m_HashTable[hashVal].push_back(&m_Particles[i]);
        m_Particles[i].HashVal = hashVal;
    }
}