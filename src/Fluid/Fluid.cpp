#include "Fluid.hpp"
#include <iostream>
#include <cmath>

glm::vec3 valueToColor(float value) {
    if (value < 0.0f || value > 1.0f) {
        //std::cerr << "Value must be between 0 and 1." << std::endl;
        return glm::vec3(0.0f); // Return black in case of error
    }

    float green = 0.5f;
    float blue = value;
    float red = 1.0f - blue;

    return glm::vec3(red, green, blue);
}

float Poly6(float r, float h)
{
    if (r > h)
    {
        return 0.0f;
    }
    else
    {
        float volume = PI * pow(h, 9);
        float value = std::max(0.0f, h * h - r * r);
        return 315.0f / (64.0f * volume) * pow(value, 3);
    }
}


float gradPoly6(float r, float h)
{
    if (r > h)
    {
        return 0.0f;
    }
    else
    {
        float volume = PI * pow(h, 9);
        float value = h * h - r * r;
        return -945.0f / (32.0f * volume) * pow(value, 2) * pow(10, 7);
    }
}

float lapPoly6(float r, float h)
{
    if (r > h)
    {
        return 0.0f;
    }
    else
    {
        float volume = PI * pow(h, 9);
        float value = h * h - r * r;
        return 945.0f / (32.0f * volume) * (h * h - 3 * r * r) * pow(10, 7);
    }
}

float spiky(float r, float h)
{
    if (r > h)
    {
        return 0.0f;
    }
    else
    {
        float volume = PI * pow(h, 6);
        float value = h - r;
        return 15.0f / (volume) * pow(value, 3);
    }
}

float gradSpiky(float r, float h)
{
    if (r > h)
    {
        return 0.0f;
    }
    else
    {
        float volume = PI * pow(h, 6) * r;
        float value = h - r;
        //std::cout << 45.0f / (volume) * pow(value, 2) << std::endl;
        return -45.0f / (volume) * pow(value, 2);
    }
}

float lapViscosity(float r, float h)
{
    if (r > h)
    {
        return 0.0f;
    }
    else
    {
        //float volume = PI * pow(h, 3);
        //float value = - pow(r, 3) / (2 * pow(h, 3)) + pow(r, 2) / (pow(h, 2)) + h / (2 * r) - 1;
        //return 15.0f / (2.0f * volume) * value;
        float factor = 45.0f / (PI * pow(h, 5));
        return factor * (h - r);
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
    float y = - 300.0f;
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
        m_Particles[i].Radius = 5.0f;

        x += spacing;
    }

    m_minBoundary = glm::vec3(-500.0f, -500.0f, 0.0f);
    m_maxBoundary = glm::vec3(500.0f, 500.0f, 0.0f);
}

void Fluid::Update()
{
    m_maxVelocity = 0.0f;
    m_minVelocity = 0.0f;
    m_maxDensity = 0.0f;
    m_minDensity = 0.0f;
    UpdateHashTable();
    CalculateDensity();
    UpdateAcceleration();

    // Update positions
    for(unsigned int i = 0; i < m_NumParticles; i++)
    {
        m_Particles[i].Vel += m_Particles[i].Acc * m_Dt;
        m_Particles[i].Pos += m_Particles[i].Vel * m_Dt;

        m_maxVelocity = std::max(m_maxVelocity, glm::length(m_Particles[i].Vel));
        m_minVelocity = std::min(m_minVelocity, glm::length(m_Particles[i].Vel));

        m_maxDensity = std::max(m_maxDensity, m_Particles[i].Density);
        m_minDensity = std::min(m_minDensity, m_Particles[i].Density);

        if(m_Particles[i].Pos.x <= m_minBoundary.x)
        {
            m_Particles[i].Pos.x = m_minBoundary.x;
            m_Particles[i].Vel.x *= -m_WallDamping;
        }
        if(m_Particles[i].Pos.x >= m_maxBoundary.x)
        {
            m_Particles[i].Pos.x = m_maxBoundary.x;
            m_Particles[i].Vel.x *= -m_WallDamping;
        }
        if(m_Particles[i].Pos.y <= m_minBoundary.y)
        {
            m_Particles[i].Pos.y = m_minBoundary.y;
            m_Particles[i].Vel.y *= -m_WallDamping;
        }
        if(m_Particles[i].Pos.y >= m_maxBoundary.y)
        {
            m_Particles[i].Pos.y = m_maxBoundary.y;
            m_Particles[i].Vel.y *= -m_WallDamping;
        }
    }
    UpdateColor();
    m_Circle->UpdateInstanceData(&m_Particles);
}

void Fluid::PrintVelocities()
{
    for(unsigned int i = 0; i < m_NumParticles; i++)
    {
        std::cout << glm::length(m_Particles[i].Vel) << ", ";
    }
}

void Fluid::UpdateColor()
{
    
    for(unsigned int i = 0; i < m_NumParticles; i++)
    {
        float value = (glm::length(m_Particles[i].Vel) - m_minVelocity) / (m_maxVelocity - m_minVelocity);
        m_Particles[i].Color = valueToColor(value);
    }

    /*
    for(unsigned int i = 0; i < m_NumParticles; i++)
    {
        float value = (m_Particles[i].Density - m_minDensity) / (m_maxDensity - m_minDensity);
        m_Particles[i].Color = valueToColor(value);
    }
    */
    /*for(auto it : m_HashTable)
    {
        int hashVal = it.first;
        float value = hashVal / 1000.0f;
        for(unsigned int i = 0; i < it.second.size(); i++)
        {
            it.second[i]->Color = valueToColor(value);
        }
    }*/
}

void Fluid::UpdateAcceleration()
{
    // Calculate acceleration vector for each particle
    for(int i = 0; i < m_NumParticles; i++)
    {
        glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);

        // Neighborhood search

        // Loop over 3x3 grid of cells around particle

        int X0 = (m_Particles[i].Pos.x - m_minBoundary.x) / m_cellSize;
        int Y0 = (m_Particles[i].Pos.y - m_minBoundary.y) / m_cellSize;

        float density = m_Particles[i].Density;
        float pressure = m_K * (density - m_RestDensity);
        //std::cout << pressure << std::endl;

        for(int j = 0; j < 9; j++)
        {
            // Get current cell offset

            int X = X0 + j % 3 - 1;
            int Y = Y0 + j / 3 - 1;

            // Get hash value of current cell

            size_t hashVal = Hash(X, Y, m_NumCells);

            //size_t hashVal = Hash(X, Y, 25 * 25);

            // If the cell exists, loop over particles in the cell

            if(m_HashTable.find(hashVal) != m_HashTable.end())
            {

                // Calculate force on particle

                for(unsigned int k = 0; k < m_HashTable[hashVal].size(); k++)
                {
                    // Pressure
                    float currDensity = m_HashTable[hashVal][k]->Density;
                    float currPressure = m_K * (currDensity - m_RestDensity);
                    float r = glm::length(m_Particles[i].Pos - m_HashTable[hashVal][k]->Pos);
                    
                    if(r > 0.0f)
                    {
                        // Direction vector from particle to neighbor
                        glm::vec3 dir = glm::normalize(m_Particles[i].Pos - m_HashTable[hashVal][k]->Pos);
                        //force += - 200.0f / (dist * dist) * dir;

                        //Pressure
                        force += - 1.0f * 750.0f * m_Mass * ((pressure + currPressure) / (2 * currDensity)) * gradPoly6(r, m_SmoothingRadius) * dir;
                        //std::cout << - 1.0f * m_Mass * ((pressure + currPressure) / (2 * currDensity)) * gradPoly6(r, m_SmoothingRadius) << std::endl;
                        
                        // Viscosity
                        force += 100 * m_Viscosity * m_Mass * (m_HashTable[hashVal][k]->Vel - m_Particles[i].Vel) / currDensity * lapViscosity(r, m_SmoothingRadius);
                    }
                }
            }
        }

        // Gravity
        force += -1.0f * m_Mass * glm::vec3(0.0f, m_G, 0.0f);

        // Update acceleration
        m_Particles[i].Acc = force / m_Mass;
    }
}

void Fluid::CalculateDensity()
{
    for(unsigned int i = 0; i < m_NumParticles; i++)
    {
        //int X0 = (int)((m_Particles[i].Pos.x - m_minBoundary.x) / m_SmoothingRadius);
        //int Y0 = (int)((m_Particles[i].Pos.y - m_minBoundary.y) / m_SmoothingRadius);
        int X0 = (m_Particles[i].Pos.x - m_minBoundary.x) / m_cellSize;
        int Y0 = (m_Particles[i].Pos.y - m_minBoundary.y) / m_cellSize;
        float density = 0.0f;

        // Loop over 3x3 grid of cells around particle
        for(int j = 0; j < 9; j++)
        {
            // Get current cell offset
            int X = X0 + j % 3 - 1;
            int Y = Y0 + j / 3 - 1;

            // Get hash value of current cell
            size_t hashVal = Hash(X, Y, m_NumCells);
            
            //size_t hashVal = Hash(X, Y, 25 * 25);

            // If the cell exists, loop over particles in the cell
            if(m_HashTable.find(hashVal) != m_HashTable.end())
            {
                for(unsigned int k = 0; k < m_HashTable[hashVal].size(); k++)
                {
                    float r = glm::length(m_Particles[i].Pos - m_HashTable[hashVal][k]->Pos);
                    density += m_Mass * Poly6(r, m_SmoothingRadius) + 0.0001f;
                } 
            }
        }
        m_Particles[i].Density = density;
    }
}

void Fluid::UpdateHashTable()
{
    m_NumCells = floor((m_maxBoundary.x - m_minBoundary.x) / m_cellSize) * floor((m_maxBoundary.y - m_minBoundary.y) / m_cellSize);
    //int numCells = 25 * 25;

    m_HashTable.clear();
    for(unsigned int i = 0; i < m_NumParticles; i++)
    {
        int X = (m_Particles[i].Pos.x - m_minBoundary.x) / m_cellSize;
        int Y = (m_Particles[i].Pos.y - m_minBoundary.y) / m_cellSize;
        size_t hashVal = Hash(X, Y, m_NumCells);
        m_HashTable[hashVal].push_back(&m_Particles[i]);
    }
}