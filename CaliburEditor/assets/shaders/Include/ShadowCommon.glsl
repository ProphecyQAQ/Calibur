layout(std140, binding = 5) uniform LightSpaceMatrices
{
	mat4 u_lightSpaceMatrices[5];
	float u_CascadePlaneDistances[5];
};

layout(binding = 8) uniform sampler2DArray u_DirCSM;
