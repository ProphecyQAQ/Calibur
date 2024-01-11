
// Camera
layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};


struct DirectionLight
{
	vec3 Radiance;
	vec3 Direction;
	float Intensity;
};

layout(std140, binding = 3) uniform DirectionalLightData
{
	DirectionLight u_DirectionalLight;
};
