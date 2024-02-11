#include "../Public/Renderer.h"
#include "../Public/IndexBuffer.h"
#include "../Public/VertexBuffer.h"
#include "../Public/VertexBufferLayout.h"
#include "../Public/Model.h"


#include <iostream>
#include <glm/gtc/type_ptr.hpp>

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[ERROR: ]" << error << " " <<
			function << " " << file << " " << line << std::endl;
		return false;
	}
	return true;
}

void Renderer::Clear() const
{
	// Clean the back buffer and assign the new color to it
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	GLCall(glClearColor(0.8f, 0.8f, 0.8f, 1.0f));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();

	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}
