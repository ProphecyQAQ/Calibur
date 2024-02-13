#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(location = 0) out vec2 TexCoord;

void main()
{
	TexCoord = a_TexCoord;
	gl_Position = vec4(a_Position, 1.0);
}

#type tessCtrl
#version 450 core

#include "Buffer.glsl"

layout (vertices=4) out;

layout(location = 0) in vec2 TexCoord[];
layout(location = 0) out vec2 TextureCoord[];

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

	// invocation zero controls tessellation levels for the entire patch
    if (gl_InvocationID == 0)
    {
		const int MIN_TESS_LEVEL = 4;
		const int MAX_TESS_LEVEL = 64;
		const float MIN_DISTANCE = 20;
		const float MAX_DISTANCE = 800;

		vec4 eyeSpacePos00 = u_ViewMatrix * u_Transform * gl_in[0].gl_Position;
		vec4 eyeSpacePos01 = u_ViewMatrix * u_Transform * gl_in[1].gl_Position;
		vec4 eyeSpacePos10 = u_ViewMatrix * u_Transform * gl_in[2].gl_Position;
		vec4 eyeSpacePos11 = u_ViewMatrix * u_Transform * gl_in[3].gl_Position;

		float distance00 = clamp((abs(eyeSpacePos00.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
		float distance01 = clamp((abs(eyeSpacePos01.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
		float distance10 = clamp((abs(eyeSpacePos10.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
		float distance11 = clamp((abs(eyeSpacePos11.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);

		float tessLevel0 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance10, distance00) );
		float tessLevel1 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance00, distance01) );
		float tessLevel2 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance01, distance11) );
		float tessLevel3 = mix( MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance11, distance10) );

		gl_TessLevelOuter[0] = tessLevel0;
		gl_TessLevelOuter[1] = tessLevel1;
		gl_TessLevelOuter[2] = tessLevel2;
		gl_TessLevelOuter[3] = tessLevel3;

		gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
		gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);
    }
}

#type tessEval
#version 450 core

layout(quads, fractional_odd_spacing, ccw) in;

layout(binding = 13) uniform sampler2D u_HeightMap;

#include "Buffer.glsl"

layout(location = 0) in vec2 TextureCoord[];

layout(location = 0) out float Height;

void main()
{
	// get patch coordinate
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

	// retrieve control point texture coordinates
    vec2 t00 = TextureCoord[0];
    vec2 t01 = TextureCoord[1];
    vec2 t10 = TextureCoord[2];
    vec2 t11 = TextureCoord[3];

	// bilinearly interpolate texture coordinate across patch
    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    vec2 texCoord = (t1 - t0) * v + t0;

    // lookup texel at patch coordinate for height and scale + shift as desired
    Height = texture(u_HeightMap, texCoord).y * 64.0 - 16.0;

    // retrieve control point position coordinates
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    // compute patch surface normal
    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec4 normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );

	// bilinearly interpolate position coordinate across patch
    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0;

	// displace point along normal
    p += normal * Height;

    // ----------------------------------------------------------------------
    // output patch point position in clip space
	gl_Position = u_ViewProjection * u_Transform * p;
}

#type fragment
#version 450 core

layout(location = 0) in float Height;

layout(location = 0) out vec4 FragColor;

void main()
{
	float h = (Height + 16)/64.0f;
	FragColor = vec4(h, h, h, 1.0);
}
