#version 410

// attributes
layout(location = 0) in vec3 in_position;

// uniforms
uniform mat4 model;
uniform mat4 light_view_proj;

void main() 
{
    vec4 worldPos = model * vec4(in_position, 1.0);
    gl_Position = light_view_proj * worldPos;
}