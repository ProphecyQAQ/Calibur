layout(binding = 0) uniform sampler2D u_DiffuseTexture;
layout(binding = 1) uniform sampler2D u_NormalTexture;
layout(binding = 2) uniform sampler2D u_SpecTexture;
layout(binding = 3) uniform sampler2D u_RoughnessTexture;
layout(binding = 4) uniform sampler2D u_AoTexture;

layout(binding = 5) uniform samplerCube u_EnvPrefilterTex;
layout(binding = 6) uniform samplerCube u_EnvIrradianceTex;

layout(binding = 7) uniform sampler2D u_BrdfLut;

const float PI = 3.14159265359;


struct PBRParameters
{
	vec3 Albedo;
	float Metallic;
	float Roughness;
}m_Params;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 myMix(vec3 x, vec3 y, float a) {
    return x * (1.0 - a) + y * a;
}

float CalculateLOD(vec2 texCoord)
{
	vec2 size = vec2(textureSize(u_DiffuseTexture, 0));
	vec2 dx_vtc = dFdx(texCoord * size);
	vec2 dy_vtc = dFdy(texCoord * size);

	float dxdy_max_vtc = max(dot(dx_vtc, dx_vtc), dot(dy_vtc, dy_vtc));
	return 0.5 * log2(dxdy_max_vtc);
}
