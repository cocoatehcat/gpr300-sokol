#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// Uniforms
uniform sampler2D screen;  
uniform float gamma;

void main()
{
    vec3 color = texture(screen, vs_texcoord).rgb;
    FragColor.rgb = pow(color, vec3(1.0/gamma));
}