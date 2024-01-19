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
	vec3 Albedo;
	float Metallic;
	float Roughness;
	float Emission;
	bool useNormalMap;
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

void main()
{
	//Normal map
	vec3 normal = vec3(0.0);
	if (useNormalMap)
	{
		normal = texture(u_NormalTexture, Input.texCoord).rgb;
		normal = normalize(normal * 2.0 - 1.0);
		normal = normalize(Input.tbn * normal);
	}
	else normal = normalize(Input.worldNormal);

	vec3 camPos = u_CameraPosition.xyz;

	vec3 lightDir = normalize(u_DirectionalLight.Direction);
	vec3 viewDir = normalize(camPos - Input.worldPosition);
	vec3 halfDir = normalize(lightDir + viewDir);

	vec3 diffuseColor = texture(u_DiffuseTexture, Input.texCoord).rgb;
	vec3 specColor = texture(u_SpecTexture, Input.texCoord).rgb;

	vec3 irradiance = texture(u_EnvIrradianceTex, normal).rgb;

	vec3 F0 = vec3(0.04);
	F0 = myMix(F0, diffuseColor, Metallic);

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(normal, halfDir, Roughness);
	float G   = GeometrySmith(normal, viewDir, lightDir, Roughness);
	vec3 F    = fresnelSchlick(clamp(dot(normal, viewDir), 0.0, 1.0), F0);

	vec3 numerator    = NDF * G * F; 
	float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
	vec3 specular = numerator / denominator;

	vec3 kS = F; // specular fresnel
	vec3 kD = vec3(1.0) - kS;

	kD *= 1.0 - Metallic;

	float NdotL = max(dot(normal, lightDir), 0.0);

	vec3 Lo = (kD * diffuseColor / PI + specular) * u_DirectionalLight.Radiance * NdotL; 

	//ambient
	vec3 ambient = irradiance * diffuseColor * kD;

	Lo = Lo + ambient;
	// HDR tonemapping
    Lo = Lo / (Lo+ vec3(1.0));
    // gamma correct
    Lo = pow(Lo, vec3(1.0/2.2)); 
	
	FragColor = vec4(Lo, 1.0);
	ID = -1;
}
