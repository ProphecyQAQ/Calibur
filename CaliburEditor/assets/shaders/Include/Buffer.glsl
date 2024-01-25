
// Camera

layout(std140, binding = 0) uniform SceneData 
{
	mat4 u_ViewProjection;
	vec4 u_CameraPosition;
};


struct DirectionLight
{
	vec3 Radiance;
	float Intensity;
	vec3 Direction;
};

layout(std140, binding = 3) uniform DirectionalLightData
{
	uint u_DirectionalLightCount;
	DirectionLight u_DirectionalLights[4];
};

struct PointLight
{
	vec3 Radiance;
	float Intensity;
	vec3 Position;
	float Radius;
	float SourceSize;
	uint CastShadow;
};

layout(std140, binding = 4) uniform PointLightData
{
	uint u_PointLightCount;
	PointLight u_PointLights[16];
};