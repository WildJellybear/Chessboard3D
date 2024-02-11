#pragma once

#include "Typedef.h"
#include "Renderer.h"
#include <string>

class Texture
{
private:
	uint m_RendererID;
	std::string m_FilePath;
	uchar* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;

public:
	Texture(const std::string& path);
	~Texture();

	void Bind(uint slot = 0) const;
	void UnBind() const;

	inline int GetWidth() const { return m_Width; };
	inline int GetHeight() const { return m_Height; };

};
