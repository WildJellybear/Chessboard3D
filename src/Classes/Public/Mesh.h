#pragma once

#include <vector>
#include <string>
#include "Shader.h"
#include "Typedef.h"
#include <glm/gtc/type_ptr.hpp>
#include <assimp/scene.h>
#include "IndexBuffer.h"

class VertexArray;
class VertexBuffer;
class VertexBufferLayout;

class Mesh
{
protected:
	VertexArray* m_VA;
	VertexBufferLayout* m_VBL;
	VertexBuffer* m_VB;
	IndexBuffer* m_IB;
private:

	void LoadMesh(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh);
public:
	Mesh() { }
	Mesh(const std::string& path);
	~Mesh();

	void Bind() const;
	void UnBind() const;

	uint GetIndexCount() const { return m_IB->GetCount(); };
};


