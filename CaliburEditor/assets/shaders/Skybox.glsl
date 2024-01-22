// Basic Texture Shader

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

#include "Buffer.glsl"

layout(location = 0) out vec3 v_Position;

void main()
{
	vec4 position = vec4(a_Position.xy, 0.0, 1.0);

	gl_Position = position;
	v_Position = (inverse(u_ViewProjection) * vec4(a_Position, 1.0)).xyz;
}

#type fragment
#version 450 core

layout(location = 0) in vec3 v_Position;

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

layout(binding = 1) uniform samplerCube u_TextureCube;

void main()
{
	color = textureLod(u_TextureCube, v_Position, 0.0);

	color2 = -1;
}
