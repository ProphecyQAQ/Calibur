const vec2 Halton_2_3[8] =
{
    vec2(0.0f, -1.0f / 3.0f),
    vec2(-1.0f / 2.0f, 1.0f / 3.0f),
    vec2(1.0f / 2.0f, -7.0f / 9.0f),
    vec2(-3.0f / 4.0f, -1.0f / 9.0f),
    vec2(1.0f / 4.0f, 5.0f / 9.0f),
    vec2(-1.0f / 4.0f, -5.0f / 9.0f),
    vec2(3.0f / 4.0f, 1.0f / 9.0f),
    vec2(-7.0f / 8.0f, 7.0f / 9.0f)
};

layout(std140, binding = 6) uniform TaaData
{
	mat4 u_PreView;
	mat4 u_PreProjection;
	float u_ScreenWidth;
	float u_ScreenHeight;
	float u_DeltaTime;
	uint u_JitterIndex;
};


layout(binding = 9)  uniform sampler2D u_PreviousFrame;
layout(binding = 10) uniform sampler2D u_CurrentFrame;
layout(binding = 11) uniform sampler2D u_MotionVector;
layout(binding = 12) uniform sampler2D u_CurrentDepth;
