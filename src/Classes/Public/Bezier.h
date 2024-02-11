#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

#include "Renderer.h"
#include "Mesh.h"

#define BEZIER_DEGREE 4

class Bezier : Mesh
{
public:
	int m_TriangulationPrecision;
private:
	/*float m_ControlPoints[BEZIER_DEGREE][BEZIER_DEGREE] = {
		{0.f, 0.f, 0.f, 0.f},
		{0.f, -0.3f, 0.3f, 0.f},
		{0.f, 0.3f, -0.3f, 0.f},
		{0.f, 0.f, 0.f, 0.f}
	};*/
	float m_ControlPoints[BEZIER_DEGREE][BEZIER_DEGREE] = { 0 };
	float m_ChangeSpeed[2][2] = {
		{0.02f, 0.04f},
		{0.07f, 0.1f}
	};
	float m_MaxHeight = 0.15f;
	std::vector<float> m_PositionTextureNormal;
	float m_ZArray[];

public:
	Bezier(int prcision = 50);
	float CalZ(float x, float y);
	void Tick(float interval);

	float GetVertexZ(int i, int j);

private:
	void UpdateArrays();
	glm::vec3 CalN(float x, float y);
	float B(int i, int n, float t);
	int factorial(int n)
	{
		int result = 1;
		for (int i = 1; i <= n; i++)
			result *= i;
		return result;
	}

	void SetVertexZ(int i, int j, float value);

	void SetVertexN(int i, int j, glm::vec3 value);
};