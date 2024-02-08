// Basic Texture Shader

#type vertex
#version 450 core

#include "Buffer.glsl"

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

	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

#include "Buffer.glsl"

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

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

layout(set = 0, binding = 0) uniform sampler2D u_DiffuseTexture;
layout(set = 0, binding = 1) uniform sampler2D u_NormalTexture;
layout(set = 0, binding = 2) uniform sampler2D u_SpecTexture;
layout(set = 0, binding = 3) uniform sampler2D u_RoughnessTexture;

void main()
{
	//Normal map
	vec3 normal = texture(u_NormalTexture, Input.texCoord).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(Input.tbn * normal);

	vec3 lightDir = normalize(u_DirectionalLights[0].Direction);
	vec3 viewDir = normalize(u_CameraPosition.xyz - Input.worldPosition);

	vec3 diffuseColor = texture(u_DiffuseTexture, Input.texCoord).rgb;
	//ambient
	vec3 ambient = 0.05 * diffuseColor;

	vec3 diffuse = Albedo * u_DirectionalLights[0].Radiance * max(dot(normal, lightDir), 0.0);
	diffuse = diffuse * diffuseColor;
	
	vec3 halfDir = normalize(lightDir + viewDir);
	vec3 spec = pow(max(dot(normal, halfDir), 0.0), 32.0) * u_DirectionalLights[0].Radiance;
	
	color = vec4(ambient + diffuse + spec, 1.0);

	color2 = -1;
}
