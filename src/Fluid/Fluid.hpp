#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "Particle.hpp"
#include "../render_geom/Circle/Circle.hpp"

#include <unordered_map>

double Poly6(double r, double h);

float gradPoly6(float r, float h);

float lapPoly6(float r, float h);

size_t Hash(glm::vec3 pos);

class Fluid
{
public:

    Fluid(unsigned int numParticles, unsigned int initCols);
    std::vector<Particle>* GetParticles() { return &m_Particles; }

    void BindCircle(Circle* circle) { m_Circle = circle; }
    void Update();

    // Debug Function
    void PrintVelocities();

    void Setm_K(float k) { m_K = k; }
    void Setm_SmoothingRadius(float smoothingRadius) { m_SmoothingRadius = smoothingRadius; 
        m_cellSize = m_cellSizeMultiplier * m_SmoothingRadius; }
    void Setm_Mass(float mass) { m_Mass = mass; }
    void Setm_Viscosity(float viscosity) { m_Viscosity = viscosity; }
    void Setm_G(float g) { m_G = g; }
    void Setm_RestDensity(float restDensity) { m_RestDensity = restDensity; }
    void Setm_WallDamping(float wallDamping) { m_WallDamping = wallDamping; }
    void Setm_Boundary(glm::vec3 min, glm::vec3 max) { m_minBoundary = min; m_maxBoundary = max; }

    float* Getm_K() { return &m_K; }
    float* GetSmoothingRadius() { return &m_SmoothingRadius; }
    float* GetMass() { return &m_Mass; }
    float* Getm_Viscosity() { return &m_Viscosity; }
    float* Getm_G() { return &m_G; }
    float* Getm_RestDensity() { return &m_RestDensity; }
    float* Getm_WallDamping() { return &m_WallDamping; }

private:
    std::vector<Particle> m_Particles;
    unsigned int m_NumParticles;
    unsigned int m_InitCols;
    unsigned int m_NumCells;

    Circle* m_Circle;

    glm::vec3 m_minBoundary;
    glm::vec3 m_maxBoundary;

    float m_Dt = 1 / 144.0f;
    float m_SmoothingRadius = 15.0f;
    float m_Mass = 1.0f;
    float m_K = 0.2f;
    float m_cellSizeMultiplier = 1.1f;
    float m_cellSize = m_cellSizeMultiplier * m_SmoothingRadius;
    float m_maxVelocity = 0.0f;
    float m_minVelocity = 0.0f;
    float m_maxDensity = 0.0f;
    float m_minDensity = 0.0f;
    float m_Viscosity = 5.0f;
    float m_G = 0.0f;
    float m_RestDensity = 0.0f;
    float m_WallDamping = 0.9f;

    std::unordered_map<size_t, std::vector<Particle*>> m_HashTable;

    void UpdateColor();
    void UpdateAcceleration();
    void CalculateDensity();
    void GetProperty(Particle* p);
    void UpdateHashTable();
};