#include "../Public/Bezier.h"
#include "../Public/VertexArray.h"
#include "../Public/VertexBuffer.h"
#include "../Public/VertexBufferLayout.h"

Bezier::Bezier(int precision) :
	m_TriangulationPrecision(precision)
{
	float dist = 1.0f / m_TriangulationPrecision;
	float curX = 0;
	float curY = 0;
	m_PositionTextureNormal = std::vector<float>();
	for (int i = 0; i < m_TriangulationPrecision + 1; i++)
	{
		for (int j = 0; j < m_TriangulationPrecision + 1; j++)
		{
			// inits flat bezier
			// coords
			m_PositionTextureNormal.push_back(curX);
			m_PositionTextureNormal.push_back(CalZ(curX, curY));
			m_PositionTextureNormal.push_back(curY);
			// texture
			m_PositionTextureNormal.push_back((float)i / m_TriangulationPrecision);
			m_PositionTextureNormal.push_back((float)j / m_TriangulationPrecision);
			// normal
			glm::vec3 normal = CalN(curX, curY);
			m_PositionTextureNormal.push_back(normal.x);
			m_PositionTextureNormal.push_back(normal.y);
			m_PositionTextureNormal.push_back(normal.z);

			curY += dist;
		}
		curY = 0;
		curX += dist;
	}
	// index buffer
	std::vector<uint> indices;
	for (int i = 0; i < m_TriangulationPrecision; i++)
	{
		for (int j = 0; j < m_TriangulationPrecision; j++)
		{
			indices.push_back(i + j * (m_TriangulationPrecision + 1)); //tl
			indices.push_back(i + j * (m_TriangulationPrecision + 1) + 1); //tr
			indices.push_back(i + (j + 1) * (m_TriangulationPrecision + 1)); //bl
			indices.push_back(i + j * (m_TriangulationPrecision + 1) + 1); //tr
			indices.push_back(i + (j + 1) * (m_TriangulationPrecision + 1) + 1); //br
			indices.push_back(i + (j + 1) * (m_TriangulationPrecision + 1));//bl
		}
	}

	m_VA = new VertexArray();
	m_VBL = new VertexBufferLayout();
	m_IB = new IndexBuffer(indices.data(), indices.size());
	m_VB = new VertexBuffer(m_PositionTextureNormal.data(), m_PositionTextureNormal.size() * sizeof(float));

	// positions
	m_VBL->Push<float>(3);
	// texture
	m_VBL->Push<float>(2);
	// normals
	m_VBL->Push<float>(3);

	m_VA->AddBuffer(*m_VB, *m_VBL);
}

float Bezier::CalZ(float x, float y)
{
	int len = BEZIER_DEGREE;
	double z = 0;
	for (int i = 0; i < len; i++)
		for (int j = 0; j < len; j++)
			z += m_ControlPoints[i][j] * B(i, len - 1, x) * B(j, len - 1, y);

	return z;
}

void Bezier::Tick(float interval)
{
	for (int i = 1; i < 3; i++)
		for (int j = 1; j < 3; j++)
		{
			m_ControlPoints[i][j] += interval * m_ChangeSpeed[i - 1][j - 1];
			if (std::abs(m_ControlPoints[i][j]) > m_MaxHeight)
				m_ChangeSpeed[i - 1][j - 1] = -m_ChangeSpeed[i - 1][j - 1];
		}
	UpdateArrays();
}

void Bezier::UpdateArrays()
{
	for (int i = 0; i < m_TriangulationPrecision + 1; i++)
	{
		for (int j = 0; j < m_TriangulationPrecision + 1; j++)
		{
			float x = (float)i / m_TriangulationPrecision;
			float y = (float)j / m_TriangulationPrecision;
			float newZ = CalZ(x, y);
			SetVertexZ(i, j, newZ);
			glm::vec3 newN = CalN(x, y);
			SetVertexN(i, j, newN);
		}
	}

	delete m_VB;
	m_VB = new VertexBuffer(m_PositionTextureNormal.data(), m_PositionTextureNormal.size() * sizeof(float));
	m_VA->AddBuffer(*m_VB, *m_VBL);
}

glm::vec3 Bezier::CalN(float x, float y)
{
	int n = BEZIER_DEGREE - 1;
	int m = n;

	glm::vec3 Px(1.f, 0.f, 0.f);
	glm::vec3 Py(0.f, 1.f, 0.f);

	for (int i = 0; i <= n - 1; i++)
		for (int j = 0; j <= m; j++)
			Px.z += (m_ControlPoints[i + 1][j] - m_ControlPoints[i][j]) * B(i, n - 1, x) * B(j, m, y);

	Px.z *= n;

	for (int i = 0; i <= n; i++)
		for (int j = 0; j <= m - 1; j++)
			Py.z += (m_ControlPoints[i][j + 1] - m_ControlPoints[i][j]) * B(i, n, x) * B(j, m - 1, y);

	Py.z *= m;

	glm::vec3 N = glm::cross(Px, Py);
	N = glm::normalize(N);
	// change because y is up in my scene
	float tmp = N.y;
	N.y = N.z;
	N.z = tmp;
	return N;
}

float Bezier::B(int i, int n, float t)
{
	double a = pow(t, i);
	double b = pow(1 - t, n - i);
	return a * b * factorial(n) / (factorial(i) * factorial(n - i));
}

float Bezier::GetVertexZ(int i, int j)
{
	int index = (i * m_TriangulationPrecision + j) * 8 + 1; // (row * precision + col) * elems_per_vertex + y_index_in_layout
	return m_PositionTextureNormal[index];
}

void Bezier::SetVertexZ(int i, int j, float value)
{
	int index = (i * m_TriangulationPrecision + j) * 8 + 1; // (row * precision + col) * elems_per_vertex + y_index_in_layout
	m_PositionTextureNormal[index] = value;
}

void Bezier::SetVertexN(int i, int j, glm::vec3 value)
{
	int index = (i * m_TriangulationPrecision + j) * 8 + 5; // (row * precision + col) * elems_per_vertex + normal_x_index_in_layout
	m_PositionTextureNormal[index] = value.x;
	m_PositionTextureNormal[index + 1] = value.y;
	m_PositionTextureNormal[index + 2] = value.z;
}
