// Basic Texture Shader

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

layout(std140, binding = 1) uniform Transform
{
	mat4 u_Transform;
};

struct VertexOutput
{
	vec3 worldPosition;
	vec2 texCoord;
};

layout (location = 0) out VertexOutput Output;

void main()
{
	Output.worldPosition = mat3(u_Transform) * a_Position;
	Output.texCoord = a_TexCoord;

	gl_Position = vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

struct VertexOutput
{
	vec3 worldPosition;
	vec2 texCoord;
};

layout (location = 0) in VertexOutput Input;

layout(set = 0, binding = 0) uniform sampler2D u_AlbedoTexture;
layout(set = 0, binding = 1) uniform sampler2D u_NormalTexture;
layout(set = 0, binding = 2) uniform sampler2D u_MetallicTexture;
layout(set = 0, binding = 3) uniform sampler2D u_RoughnessTexture;

void main()
{
	color = texture(u_AlbedoTexture, Input.texCoord);
	//color = vec4(0.1, 0.1, 0.1, 1.0);

	color2 = -1;
}
