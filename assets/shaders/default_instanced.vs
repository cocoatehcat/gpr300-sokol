#version 300 es

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec3 in_tangent;
layout(location = 4) in mat4 in_instancedMatrix;

// uniforms
uniform mat4 view_proj;
uniform mat4 model;

// varyings
out vec3 vs_position;
out vec3 vs_normal;
out vec2 vs_texcoord;
out mat3 TBN;

void main()
{
  vs_position = vec3(in_instancedMatrix * vec4(in_position, 1.0));
  vs_normal = in_normal;
  vs_texcoord = in_texcoord;
  gl_Position = view_proj * vec4(vs_position, 1.0);

  vec3 T = normalize(vec3(model * vec4(in_tangent, 0.0)));
  vec3 N = normalize(vec3(model * vec4(in_normal, 0.0)));
  vec3 B = cross(N, T);
  TBN = mat3(T, B, N);
}