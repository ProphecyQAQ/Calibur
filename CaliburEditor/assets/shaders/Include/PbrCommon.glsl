layout(binding = 0) uniform sampler2D u_DiffuseTexture;
layout(binding = 1) uniform sampler2D u_NormalTexture;
layout(binding = 2) uniform sampler2D u_SpecTexture;
layout(binding = 3) uniform sampler2D u_RoughnessTexture;
layout(binding = 4) uniform sampler2D u_AoTexture;

layout(binding = 5) uniform samplerCube u_EnvPrefilterTex;
layout(binding = 6) uniform samplerCube u_EnvIrradianceTex;

layout(binding = 7) uniform sampler2D u_BrdfLut;
