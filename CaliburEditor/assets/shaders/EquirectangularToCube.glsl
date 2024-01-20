// Basic Texture Shader

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

layout(std140, binding = 30) uniform Cameras
{
	mat4 u_ViewProjection;
};

layout(location = 0) out vec3 v_Position;

void main()
{
	v_Position = a_Position;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) in vec3 v_Position;

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_EquirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
	vec2 uv = SampleSphericalMap(normalize(v_Position)); // make sure to normalize localPos
    vec3 color = texture(u_EquirectangularMap, uv).rgb;

	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}
