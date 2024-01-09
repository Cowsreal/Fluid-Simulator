#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "Particle.hpp"
#include "../render_geom/Circle/Circle.hpp"

#include <unordered_map>

double Poly6(double r, double h);

float gradPoly6(float r, float h);

size_t Hash(glm::vec3 pos);

class Fluid
{
public:

    Fluid(unsigned int numParticles, unsigned int initCols);
    std::vector<Particle>* GetParticles() { return &m_Particles; }

    void BindCircle(Circle* circle) { m_Circle = circle; }
    void Update();

private:
    std::vector<Particle> m_Particles;
    unsigned int m_NumParticles;
    unsigned int m_InitCols;

    Circle* m_Circle;

    glm::vec2 m_minBoundary;
    glm::vec2 m_maxBoundary;

    float m_Dt = 1 / 120.0f;
    float m_SmoothingRadius = 30.0f;
    float m_Mass = 1.0f;
    float m_K = 0.1f;

    std::unordered_map<size_t, std::vector<Particle*>> m_HashTable;

    void UpdateAcceleration();
    void CalculateDensity();
    void GetProperty(Particle* p);
    void UpdateHashTable();
};