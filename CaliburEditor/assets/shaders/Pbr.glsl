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
	if (useNormalMap == 1u)
	{
		normal = texture(u_NormalTexture, Input.texCoord).rgb;
		normal = normalize(normal * 2.0 - 1.0);
		normal = normalize(Input.tbn * normal);
	}

	vec3 camPos = u_CameraPosition.xyz;
	vec3 viewDir = normalize(camPos - Input.worldPosition);
	vec3 reflectDir = normalize(reflect(-viewDir, normal));
	// Calculate Lod
	float lod = CalculateLOD(Input.texCoord);

	vec3 diffuseColor = textureLod(u_DiffuseTexture, Input.texCoord, lod).rgb;
	float metallic = textureLod(u_SpecTexture, Input.texCoord, 0.0).r * Metallic;
	float roughness = textureLod(u_RoughnessTexture, Input.texCoord, 0.0).r * Roughness;

	// Base fresnel
	vec3 F0 = vec3(0.04);
	F0 = myMix(F0, diffuseColor, metallic);

	
	vec3 Lo = vec3(0.0);
	// Calculate Directional light
	for (uint i = 0; i < u_DirectionalLightCount; i ++)
	{
		// lighting vector
		vec3 lightDir = normalize(u_DirectionalLights[0].Direction);
		vec3 halfDir = normalize(lightDir + viewDir);

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

		Lo += (kD * diffuseColor * Albedo.rgb / PI + specular) * u_DirectionalLights[0].Radiance * NdotL; 
	}
	
	vec3 test;
	for (uint i = 0; i < u_PointLightCount; i ++)
	{
		// lighting vector
		vec3 lightDir = normalize(u_PointLights[i].Position - Input.worldPosition);
		vec3 halfDir = normalize(lightDir + viewDir);
		
		float lightDistance = length(u_PointLights[i].Position - Input.worldPosition);
		float attenuation = 1/(lightDistance * lightDistance);
		vec3 radiance = u_PointLights[i].Radiance * attenuation;

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
		test = vec3(NdotL, 0.0, 0.0);

		Lo += (kD * diffuseColor * Albedo.rgb / PI + specular) * radiance * NdotL; 
	}

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
