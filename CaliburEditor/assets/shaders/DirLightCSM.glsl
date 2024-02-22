
#type vertex
#version 450 core

layout(std140, binding = 1) uniform Transform
{
	mat4 u_Transform;
	mat4 u_PreTransform;
};

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;
layout(location = 5) in ivec4 a_BoneIDs;
layout(location = 6) in vec4 a_Weights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

layout(std140, binding = 7) uniform FinalBoneMatrices
{
	mat4 u_FinalBonesMatrices[MAX_BONES];
};

void main()
{
	vec4 totalPosition = vec4(0.0);
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		if (a_BoneIDs[i] == -1) continue;
		if (a_BoneIDs[i] >= MAX_BONES) 
		{ 
			break;
		}

		vec4 localPosition = u_FinalBonesMatrices[a_BoneIDs[i]] * vec4(a_Position, 1.0);
		totalPosition += localPosition * a_Weights[i];
	}
	if (totalPosition == vec4(0.0))
	{
		totalPosition = vec4(a_Position, 1.0);
	}

	gl_Position = u_Transform  * vec4(totalPosition.xyz, 1.0);
}

#type geometry
#version 450 core

layout(triangles, invocations = 4) in;
layout(triangle_strip, max_vertices = 3) out;

#include "ShadowCommon.glsl"

void main()
{
	for (int i = 0; i < 3; i ++)
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

