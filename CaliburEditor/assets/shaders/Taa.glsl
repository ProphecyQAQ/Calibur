
#type vertex
#version 450 core

layout(location = 0) in vec3 u_Position;
layout(location = 1) in vec2 u_TexCoord;

struct VertexOutput
{
	vec2 texCoord;
};

layout(location = 0) out VertexOutput Output;

void main()
{
	Output.texCoord = u_TexCoord;
	gl_Position = vec4(u_Position, 1.0);
}

#type fragment
#version 450 core

#include "TaaCommon.glsl"

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int ID;

struct VertexOutput 
{
	vec2 texCoord;
};

layout(location = 0) in VertexOutput Input;

vec3 RGB2YCoCgR(vec3 rgbColor)
{
	vec3 YCoCgRColor;

	YCoCgRColor.y = rgbColor.r - rgbColor.b;
	float temp = rgbColor.b + YCoCgRColor.y / 2;
	YCoCgRColor.z = rgbColor.g - temp;
	YCoCgRColor.x = temp + YCoCgRColor.z / 2;

	return YCoCgRColor;
}

vec3 YCoCgR2RGB(vec3 YCoCgRColor)
{
	vec3 rgbColor;

	float temp = YCoCgRColor.x - YCoCgRColor.z / 2;
	rgbColor.g = YCoCgRColor.z + temp;
	rgbColor.b = temp - YCoCgRColor.y / 2;
	rgbColor.r = rgbColor.b + YCoCgRColor.y;

	return rgbColor;
}

float Luminance(vec3 color)
{
	return 0.25 * color.r + 0.5 * color.g + 0.25 * color.b;
}

vec3 ToneMap(vec3 color)
{
	return color / (1 + Luminance(color));
}

vec3 UnToneMap(vec3 color)
{
	return color / (1 - Luminance(color));
}

vec2 GetClosetsOffset()
{
	vec2 deltaRes = vec2(1.0/u_ScreenWidth, 1.0/u_ScreenHeight);
	float closestDepth = 1.0;
	vec2 closestUV = Input.texCoord;

	for (int i = -1; i <= 1; i ++)
	{
		for (int j = -1; j <= 1; j ++)
		{
			vec2 newUV = Input.texCoord + vec2(i, j) * deltaRes;
			float depth = texture(u_CurrentDepth, newUV).r;

			if (depth < closestDepth)
			{
				closestDepth = depth;
				closestUV = newUV;
			}
		}
	}
	return closestUV;
}

vec3 clipAABB(vec3 nowColor, vec3 preColor)
{
	vec3 aabbMin = nowColor, aabbMax = nowColor;
	vec2 deltaRes = vec2(1.0/u_ScreenWidth, 1.0/u_ScreenHeight);
	vec3 m1 = vec3(0.0), m2 = vec3(0.0);

	for (int i = -1; i <= 1; i ++)
	{
		for (int j = -1; j <= 1; j ++)
		{
			vec2 newUV = Input.texCoord + vec2(i, j) * deltaRes;
			vec3 color = RGB2YCoCgR(ToneMap(texture(u_CurrentFrame, newUV).rgb));
			m1 += color;
			m2 += color * color;
		}
	}

	// Variance clip
	const int N = 9;
	const float VarianceClipGamma = 1.0f;
	vec3 mu = m1 / N;
	vec3 sigma = sqrt(abs(m2 / N - mu * mu));
	aabbMin = mu - VarianceClipGamma * sigma;
	aabbMax = mu + VarianceClipGamma * sigma;

	// clip to center
	vec3 p_clip = 0.5 * (aabbMax + aabbMin);
	vec3 e_clip = 0.5 * (aabbMax - aabbMin);

	vec3 v_clip = preColor - p_clip;
	vec3 v_unit = v_clip.xyz / e_clip;
	vec3 a_unit = abs(v_unit);
	float ma_unit = max(a_unit.x, max(a_unit.y, a_unit.z));

	if (ma_unit > 1.0)
		return p_clip + v_clip / ma_unit;
	else
		return preColor;
}

void main()
{
	vec3 curColor = texture(u_CurrentFrame, Input.texCoord).rgb;

	vec2 velocity = texture(u_MotionVector, GetClosetsOffset()).rg;
	vec2 offsetUV = clamp(Input.texCoord - velocity, 0, 1);
	vec3 preColor = texture(u_PreviousFrame, offsetUV).rgb;

	curColor = RGB2YCoCgR(ToneMap(curColor));
	preColor = RGB2YCoCgR(ToneMap(preColor));

	preColor = clipAABB(curColor, preColor);

	curColor = UnToneMap(YCoCgR2RGB(curColor));
	preColor = UnToneMap(YCoCgR2RGB(preColor));

	FragColor = vec4(0.9 * preColor + 0.1 * curColor, 1.0);
	ID = -1;
}
