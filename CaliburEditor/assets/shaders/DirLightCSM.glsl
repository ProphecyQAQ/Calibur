
#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

layout(std140, binding = 1) uniform Transform
{
	mat4 u_Transform;
};

void main()
{
	gl_Position = u_Transform * vec4(a_Position, 1.0);
}

#type geometry
#version 450 core

layout(triangles, invocations = 5) in;
layout(triangle_strip, max_vertices = 3) out;

layout(std140, binding = 5) uniform LightSpaceMatrices
{
	mat4 u_lightSpaceMatrices[16];
};

void main()
{
	for (int i = 0; i < 6; i ++)
	{
		gl_Position = u_lightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
		gl_Layer = gl_InvocationID;
		EmitVertex();
	}
	EndPrimitive();
}

#type fragment
#version 450 core

void main()
{}

