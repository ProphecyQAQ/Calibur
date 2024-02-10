// Basic Texture Shader

#type vertex
#version 450 core

#include "Buffer.glsl"
#include "TaaCommon.glsl"

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

struct VertexOutput
{
	vec3 worldPosition;
	vec3 worldNormal;
	vec2 texCoord;
	vec3 tangent;
	vec3 bitangent;
	mat3 tbn;

	vec4 newPos;
	vec4 oldPos;
};

layout (location = 0) out VertexOutput Output;

void main()
{
	Output.worldPosition = vec3(u_Transform * vec4(a_Position, 1.0));
	Output.worldNormal = transpose(inverse(mat3(u_Transform))) * a_Normal;
	Output.texCoord = a_TexCoord;
	Output.tangent = a_Tangent;
	Output.bitangent = a_Bitangent;

	vec3 T = normalize(vec3(u_Transform * vec4(a_Tangent,   0.0)));
	vec3 B = normalize(vec3(u_Transform * vec4(a_Bitangent, 0.0)));
	vec3 N = normalize(vec3(u_Transform * vec4(a_Normal,    0.0)));
	Output.tbn = mat3(T, B, N);

	// Calculate jitter
	float deltaWidth = 1.0 / u_ScreenWidth;
	float deltaHeight = 1.0 / u_ScreenHeight;
	vec2 jitter = vec2(Halton_2_3[u_JitterIndex].x * deltaWidth, Halton_2_3[u_JitterIndex].y * deltaHeight);
	mat4 jitterMat = mat4(1.0);
	jitterMat[2][0] += jitter.x * 1.f;
	jitterMat[2][1] += jitter.y * 1.f;

	gl_Position = jitterMat * u_ViewProjection * u_Transform * vec4(a_Position, 1.0);

	// Calculate new and old pos
	Output.newPos = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
	Output.oldPos = u_PreProjection * u_PreView * u_PreTransform * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

#include "Buffer.glsl"
#include "PbrCommon.glsl"
#include "ShadowCommon.glsl"

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int ID;
layout(location = 2) out vec4 VelocityVec;

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

	vec4 newPos;
	vec4 oldPos;
};

layout (location = 0) in VertexOutput Input;

uint Index = 0;

float shadowCalculate(vec3 lightDir)
{
	// According view space z value to select cascade level
	vec4 viewSpacePos = u_ViewMatrix * vec4(Input.worldPosition, 1.0);
	float depthValue = viewSpacePos.z;

	uint layer = 1;
	const uint SHADOW_MAP_CASCADE_COUNT = 4;
	for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT - 1; i++)
	{
		if (depthValue < u_CascadePlaneDistances[i])
		{
			layer = i + 1;
		}
	}

	Index = layer;

	// Calculate shadow
	vec4 viewLightPos =  u_lightSpaceMatrices[layer] * vec4(Input.worldPosition, 1.0);

	vec3 projCoord = viewLightPos.xyz / viewLightPos.w;
	projCoord = projCoord * 0.5 + 0.5;
	float currentDepth = projCoord.z;
	
	if (currentDepth > 1.0 || currentDepth < -1.0) return 0.0;

	vec3 normal = normalize(Input.worldNormal);
	float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.005);
	//bias *= 1 / (-u_CascadePlaneDistances[layer] * 0.5f);
	
	//float depth = texture(u_DirCSM, vec3(projCoord.xy, layer)).r;
	//return step(depth + bias, currentDepth);
	float shadow = PCF_DirectionalCascadeShadow(u_DirCSM, layer, projCoord, bias);
	return shadow;
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

	vec3 specular = prefilter * (F * brdf.x + brdf.y);

	vec3 ambient = irradiance * m_Params.Albedo * kD + specular;

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
	m_Params.Albedo = diffuseColor * Albedo.rgb;
	float metallic = textureLod(u_SpecTexture, Input.texCoord, lod).r * Metallic;
	float roughness = max(textureLod(u_RoughnessTexture, Input.texCoord, lod).r * Roughness, 0.05);

	// Base fresnel
	vec3 F0 = vec3(0.04);
	F0 = myMix(F0, diffuseColor, metallic);
	
	vec3 Lo = vec3(0.0);
	// Calculate Directional light
	for (uint i = 0; i < u_DirectionalLightCount; i ++)
	{
		// lighting vector
		vec3 lightDir = normalize(u_DirectionalLights[i].Direction);
		vec3 halfDir = normalize(lightDir + viewDir);

		// Calculate shadow
		float shadow = shadowCalculate(lightDir);
		if (shadow == 1.0) continue;
		shadow = 1.0 - shadow;

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

		Lo += (kD * diffuseColor * m_Params.Albedo / PI + specular) * u_DirectionalLights[i].Radiance * u_DirectionalLights[i].Intensity * NdotL * shadow; 
	}
	

	// Calcute Point light
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

		Lo += (kD * diffuseColor * m_Params.Albedo / PI + specular) * radiance * NdotL * u_PointLights[i].Intensity; 
	}

	// IBL
	vec3 ambient = IBL(F0, normal, viewDir, reflectDir, roughness, metallic);
	
	// final color
	Lo = Lo + ambient;
	// HDR tonemapping
    Lo = Lo / (Lo + vec3(1.0));
    // gamma correct
    //Lo = pow(Lo, vec3(1.0/2.2));

	ID = -1;
	FragColor = vec4(Lo, 1.0);

	//switch (Index) {
	//	case 0:
	//	FragColor *= vec4(1.0f, 0.25f, 0.25f, 1.0f);
	//	break;
	//	case 1:
	//	FragColor *= vec4(0.25f, 1.0f, 0.25f, 1.0f);
	//	break;
	//	case 2:
	//	FragColor *= vec4(0.25f, 0.25f, 1.0f, 1.0f);
	//	break;
	//	default :
	//	FragColor *= vec4(1.0f, 1.0f, 0.25f, 1.0f);
	//	break;
	//}


	// Calculate Velocity 
	vec2 newPos = ((Input.newPos.xy / Input.newPos.w) * 0.5 + 0.5);
	vec2 oldPos = ((Input.oldPos.xy / Input.oldPos.w) * 0.5 + 0.5);

	VelocityVec = vec4(newPos - oldPos, 0.0, 1.0);
}
