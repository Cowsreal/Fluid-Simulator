#pragma once

#include "glm/glm.hpp"

#include "../../Renderer.hpp"
#include "../../VertexBuffer.hpp"
#include "../../VertexArray.hpp"
#include "../../VertexBufferLayout.hpp"
#include "../../IndexBuffer.hpp"
#include "../../Shader.hpp"
#include "../../FrameBuffer.hpp"

#include "../../Fluid/Particle.hpp"

#include <vector>
#define PI 3.14159265358979323846

struct CircleInstance {
	float position[3];
	float color[4];
	float radius;
};

class Circle
{
public:
	Circle();
	Circle(std::vector<float>, int numPoints, bool drawAsPoints, float pointSize);
	Circle(float radius);
	Circle(const std::vector<Particle>* instanceData);
	~Circle() {};
	Circle operator=(const Circle& other);

	void SetDrawAsPoints(bool drawAsPoints);
	void Draw();
	void ChangePointSize(float pointSize);
	void UpdateInstanceData(const std::vector<Particle>* instanceData);


private:
	float m_PointSize;
	bool m_DrawAsPoints;
	float m_Radius;
	bool m_IsInstanced = false;
	void GenerateCircleVertices();
	void GenerateCircleIndices();
	IndexBuffer m_IBO;
	VertexArray m_VAO;
	VertexBuffer m_VBO;
	VertexBuffer m_InstanceVBO;
	VertexBufferLayout m_VBL;
	FrameBuffer m_FBO;
	unsigned int m_NumInstances;
	std::vector<float> m_Vertices;
	std::vector<unsigned int> m_Indices;
	unsigned int m_numCols = 50;
};