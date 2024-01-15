
layout(set = 0, binding = 0) uniform sampler2D u_DiffuseTexture;
layout(set = 0, binding = 1) uniform sampler2D u_NormalTexture;
layout(set = 0, binding = 2) uniform sampler2D u_SpecTexture;
layout(set = 0, binding = 3) uniform sampler2D u_RoughnessTexture;
layout(set = 0, binding = 4) uniform sampler2D u_AoTexture;

layout(set = 0, binding = 5) uniform samplerCube u_EnvRadianceTex;
layout(set = 0, binding = 6) uniform samplerCube u_EnvIrradianceTex;

layout(set = 0, binding = 7) uniform sampler2D u_BrdfLut;
