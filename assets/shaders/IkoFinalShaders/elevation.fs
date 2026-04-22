#version 410

precision mediump float;

// attachments
out vec4 frag_position;

uniform float fog_range;
uniform float fog_height;
// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

void main()
{
    vec3 outputColor = vec3((vs_position.y * fog_range) + fog_height,0.0,0.0);
    frag_position = vec4(outputColor, 1.0);
}