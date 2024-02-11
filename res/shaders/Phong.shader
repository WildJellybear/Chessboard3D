#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec2 v_TexCoord;
out vec4 v_Color;
out vec3 v_Normal;
out vec3 FragPos;

uniform mat4 u_camMatrix;
uniform mat4 u_Model;
uniform vec4 u_Color;

void main()
{
	v_TexCoord = texCoord;
	gl_Position = u_camMatrix * u_Model * position;
	FragPos = vec3(u_Model * position);
	v_Color = u_Color;
	v_Normal = mat3(transpose(inverse(u_Model))) * normal;
};

#shader fragment
#version 330 core
layout(location = 0) out vec4 color;
	
in vec2 v_TexCoord;
in vec4 v_Color;
in vec3 v_Normal;
in vec3 FragPos;

uniform sampler2D u_Texture;
uniform vec3 u_ViewPos;
uniform bool u_FogEnabled;

struct Light{
	vec4 m_LightColor;
	vec3 m_LightPos;
	bool m_IsPointLight;
	vec3 m_LightDir;
};

#define LIGHT_COUNT 4
uniform Light lights[LIGHT_COUNT];

vec3 PointLight(Light light, vec3 worldPos, vec3 worldNormal)
{
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = light.m_LightPos - worldPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 0.032;
	float b = 0.07;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	// diffuse lighting
	vec3 Normal = normalize(worldNormal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(Normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(u_ViewPos - worldPos);
	vec3 reflectionDirection = reflect(-lightDirection, Normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (diffuse * inten + specular * inten) * light.m_LightColor * v_Color * texture(u_Texture, v_TexCoord);
}

vec3 SpotLight(Light light, vec3 worldPos, vec3 worldNormal)
{
	// controls how big the area that is lit up is
	float Cutoff = 0.8;

	vec3 LightToPixel = normalize(worldPos - light.m_LightPos);
	float SpotFactor = dot(LightToPixel, light.m_LightDir);

	if (SpotFactor > Cutoff)
	{
		vec3 Color = PointLight(light, worldPos, worldNormal);
		float SpotLightIntensity = (1.0 - (1.0 - SpotFactor) / (1.0 - Cutoff));
		return Color * SpotLightIntensity;
	}
	else
	{
		return vec3(0.0f, 0.0f, 0.0f);
	}

	// observer behind light
	//vec3 fromLight = normalize(worldPos - light.m_LightPos);
	//if (dot(fromLight, light.m_LightDir) <= test)
	//	return vec3(0.0f, 0.0f, 0.0f);

	//// diffuse lighting
	//vec3 Normal = normalize(worldNormal);
	//vec3 lightDirection = normalize(light.m_LightDir);
	//float diffuse = max(dot(Normal, lightDirection), 0.0f);

	//// specular lighting
	//float specularLight = 0.50f;
	//vec3 viewDirection = normalize(u_ViewPos - worldPos);
	//vec3 reflectionDirection = reflect(-lightDirection, Normal);

	//float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 10);
	//float specular = specAmount * specularLight;

	//// calculates the intensity of the crntPos based on its angle to the center of the light cone
	//float angle = dot(-light.m_LightPos, -lightDirection);
	//float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	//return (diffuse * inten + specular * inten) * light.m_LightColor * v_Color * texture(u_Texture, v_TexCoord);
}

float CalcFogFactor(vec3 worldPos)
{
	float fogIntensity = 0.8f;
	if (fogIntensity == 0) 
		return 1;
	
	float gradient = (fogIntensity * fogIntensity - 50 * fogIntensity + 60);
	float distance = length(u_ViewPos - worldPos);
	
	float fog = exp(-pow((gradient / distance), 4));
	fog = clamp(fog, 0.0, 1.0);
	
	return (1.0f - fog);
}
//
//vec3 CalcLight(Light light)
//{
//	if (light.m_IsPointLight)
//		return PointLight(light);
//	else
//		return SpotLight(light);
//}

void main()
{
	vec3 finalColor;
	float ambient = 0.1f;
	finalColor += vec3(v_Color * texture(u_Texture, v_TexCoord) * ambient);
	for (int i = 0; i < LIGHT_COUNT; i++)
	{
		if (lights[i].m_IsPointLight)
			finalColor += vec3(PointLight(lights[i], FragPos, v_Normal));
		else
			finalColor += vec3(SpotLight(lights[i], FragPos, v_Normal));
	}
	finalColor = clamp(finalColor, 0.0, 1.0);

	vec3 fogColor = vec3(0.9f, 0.9f, 0.9f);
	if(u_FogEnabled)
		finalColor = mix(fogColor, finalColor, CalcFogFactor(FragPos));

	color = vec4(finalColor, 1.0f);
};