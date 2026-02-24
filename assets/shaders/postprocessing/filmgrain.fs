#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// Uniforms
uniform sampler2D screen;  

void main()
{
    float noise = fract(sin(dot(vs_texcoord, vec2(12.9898, 78.233)*2.0)) * 43758.5453);
    float mdf = 0.1;
    mdf *= sin(21) + 1;
    vec3 color = texture(screen, vs_texcoord).rgb;

    FragColor = vec4(color - noise * mdf, 1.0);
}