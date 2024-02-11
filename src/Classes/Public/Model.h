#pragma once

#include <map>

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Mesh.h"
#include "memory"
#include "../../enums/ObjectType.h"

class Model
{
protected:
	std::shared_ptr<Mesh> m_Mesh;

private:
	std::shared_ptr<Texture> m_Texture;

	glm::vec3 m_Position;
	glm::vec3 m_Rotation;
	glm::vec3 m_Scale;


public:
	static std::map<int, std::shared_ptr<Mesh>> meshMap;

	Model(std::shared_ptr<Mesh> Mesh, std::shared_ptr<Texture> Tex, glm::vec3 position = glm::vec3(0.f), glm::vec3 rotation = glm::vec3(0.f), glm::vec3 scale = glm::vec3(1.f));
	~Model();

	std::shared_ptr<Mesh> GetMesh() const { return m_Mesh; };
	void Bind() const;
	void UnBind() const;

	virtual void Draw(Shader& shader) const;

	void RotateX(float angle);
	void RotateY(float angle);
	void RotateZ(float angle);

	void SetScale(glm::vec3);
	void SetScale(float);

	void SetPosition(glm::vec3 nPos) { m_Position = nPos; };
	glm::vec3 GetPosition() const { return m_Position; };

	void SetRotationY(float nRotY) { m_Rotation.y = nRotY; };
	glm::vec3 GetRotation() const { return m_Rotation; };
};

