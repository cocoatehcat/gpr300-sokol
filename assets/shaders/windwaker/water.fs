#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

// Uniforms
uniform vec3 camera;
uniform sampler2D texture128; 
uniform vec3 waterColor;

uniform float time;

void main()
{
    vec2 dir = vec2(1, 0);
    vec2 uv = vs_texcoord + vec2(time * dir);
    uv.x += 0.01 * sin(uv.x * 3.5 + time);
    uv.y += -0.35 * sin(uv.y * 1.5 + time);

    vec4 sample1 = texture(texture128, uv * 1.0);
    vec4 sample2 = texture(texture128, uv * 1.2);

    vec3 object_color = vec3((sample1 * 0.75) - (sample2 * 0.25));
    
    FragColor = vec4(object_color + waterColor, 1.0);
}