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
	mat3 tbn;
};

layout (location = 0) out VertexOutput Output;

void main()
{
	Output.worldPosition = mat3(u_Transform) * a_Position;
	Output.worldNormal = transpose(inverse(mat3(u_Transform))) * a_Normal;
	Output.texCoord = a_TexCoord;
	Output.tangent = a_Tangent;
	Output.bitangent = a_Bitangent;

	vec3 T = normalize(vec3(u_Transform * vec4(a_Tangent,   0.0)));
	vec3 B = normalize(vec3(u_Transform * vec4(a_Bitangent, 0.0)));
	vec3 N = normalize(vec3(u_Transform * vec4(a_Normal,    0.0)));
	Output.tbn = mat3(T, B, N);

	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

#include "Buffer.glsl"
#include "PbrCommon.glsl"

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int ID;

layout(std140, binding = 2) uniform MaterialUniform
{
	vec4 Albedo;
	float Metallic;
	float Roughness;
	float Emission;
	uint useNormalMap;
};

struct VertexOutput
{
	vec3 worldPosition;
	vec3 worldNormal;
	vec2 texCoord;
	vec3 tangent;
	vec3 bitangent;
	mat3 tbn;
};

layout (location = 0) in VertexOutput Input;

const float PI = 3.14159265359;
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
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

vec3 IBL(vec3 F0, vec3 normal, vec3 viewDir, vec3 reflectDir, float roughness, float metallic)
{
	vec3 irradiance = texture(u_EnvIrradianceTex, normal).rgb;
	vec3 F = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);

	vec3 kS = F;
	vec3 kD = (1.0 - kS) * (1.0 - metallic);

	const float MAX_REFLECTION_LOD = textureQueryLevels(u_EnvPrefilterTex);
	vec3 prefilter = textureLod(u_EnvPrefilterTex, reflectDir, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 brdf  = texture(u_BrdfLut, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;

	vec3 specular = (prefilter * (F * brdf.x + brdf.y));

	vec3 ambient = irradiance * Albedo.rgb * kD + specular;

	return ambient;
}

void main()
{
	//Normal map
	vec3 normal = normalize(Input.worldNormal);
	if (useNormalMap == 1)
	{
		normal = texture(u_NormalTexture, Input.texCoord).rgb;
		normal = normalize(normal * 2.0 - 1.0);
		normal = normalize(Input.tbn * normal);
	}

	vec3 camPos = u_CameraPosition.xyz;
	
	// lighting vector
	vec3 lightDir = normalize(u_DirectionalLight.Direction);
	vec3 viewDir = normalize(camPos - Input.worldPosition);
	vec3 halfDir = normalize(lightDir + viewDir);
	vec3 reflectDir = normalize(reflect(-viewDir, normal));
	
	float lod = CalculateLOD(Input.texCoord);
	
	vec3 diffuseColor = textureLod(u_DiffuseTexture, Input.texCoord, lod).rgb;
	float metallic = textureLod(u_SpecTexture, Input.texCoord, 0.0).r * Metallic;
	float roughness = textureLod(u_RoughnessTexture, Input.texCoord, 0.0).r * Roughness;
	
	// Base fresnel
	vec3 F0 = vec3(0.04);
	F0 = myMix(F0, diffuseColor, metallic);

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(normal, halfDir, roughness);
	float G   = GeometrySmith(normal, viewDir, lightDir, roughness);
	vec3 F    = fresnelSchlick(clamp(dot(normal, viewDir), 0.0, 1.0), F0);

	vec3 numerator    = NDF * G * F; 
	float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
	vec3 specular = numerator / denominator;

	vec3 kS = F; // specular fresnel
	vec3 kD = vec3(1.0) - kS;

	kD *= 1.0 - metallic;

	float NdotL = max(dot(normal, lightDir), 0.0);

	vec3 Lo = (kD * diffuseColor * Albedo.rgb / PI + specular) * u_DirectionalLight.Radiance * NdotL; 
	
	// IBL
	vec3 ambient = IBL(F0, normal, viewDir, reflectDir, roughness, metallic);
	
	// final color
	Lo = Lo + ambient;
	// HDR tonemapping
    Lo = Lo / (Lo + vec3(1.0));
    // gamma correct
    //Lo = pow(Lo, vec3(1.0/2.2));

	FragColor = vec4(Lo, 1.0);
	ID = -1;
}
