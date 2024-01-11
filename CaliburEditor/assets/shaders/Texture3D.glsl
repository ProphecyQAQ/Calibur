// Basic Texture Shader

#type vertex
#version 450 core

#include "Buffer.glsl"

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

layout(std140, binding = 1) uniform Transform
{
	mat4 u_Transform;
};

struct VertexOutput
{
	vec3 worldPosition;
	vec3 worldNormal;
	vec2 texCoord;
	vec3 tangent;
	vec3 bitangent;
};

layout (location = 0) out VertexOutput Output;

void main()
{
	Output.worldPosition = mat3(u_Transform) * a_Position;
	Output.worldNormal = transpose(inverse(mat3(u_Transform))) * a_Normal;
	Output.texCoord = a_TexCoord;
	Output.tangent = a_Tangent;
	Output.bitangent = a_Bitangent;

	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

#include "Buffer.glsl"

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

layout(std140, binding = 2) uniform MaterialUniform
{
	vec3 Albedo;
	float Metallic;
	float Roughness;
	float Emission;
	bool useNormalMap;
};

struct VertexOutput
{
	vec3 worldPosition;
	vec3 worldNormal;
	vec2 texCoord;
	vec3 tangent;
	vec3 bitangent;
};

layout (location = 0) in VertexOutput Input;

layout(set = 0, binding = 0) uniform sampler2D u_DiffuseTexture;
layout(set = 0, binding = 1) uniform sampler2D u_NormalTexture;
layout(set = 0, binding = 2) uniform sampler2D u_SpecTexture;
layout(set = 0, binding = 3) uniform sampler2D u_RoughnessTexture;

void main()
{
	//color = vec4(texture(u_DiffuseTexture, Input.texCoord).rgb, 1.0);
	vec3 diffuseColor = texture(u_DiffuseTexture, Input.texCoord).rgb * u_DirectionalLight.Radiance;
	color = vec4(diffuseColor, 1.0);
	//color = vec4(0.1, 0.1, 0.1, 1.0);

	color2 = -1;
}
