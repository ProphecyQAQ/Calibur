
// Camera

layout(std140, binding = 0) uniform SceneData 
{
	mat4 u_ViewProjection;
	vec4 u_CameraPosition;
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
