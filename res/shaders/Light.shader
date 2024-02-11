#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colorV;
layout(location = 2) in vec2 texCoord;

out vec2 v_TexCoord;
out vec4 v_Color;
// Imports the camera matrix from the main function
uniform mat4 u_camMatrix;
uniform mat4 u_Model;

void main()
{
	v_TexCoord = texCoord;
	gl_Position = u_camMatrix * u_Model * position;
	v_Color = colorV;
};

#shader fragment
#version 330 core
layout(location = 0) out vec4 color;
	
in vec2 v_TexCoord;
in vec4 v_Color;
uniform vec4 u_Color;

void main()
{
	color = u_Color;
};