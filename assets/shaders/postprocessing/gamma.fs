#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// Uniforms
uniform sampler2D screen;  

void main()
{
    float gamma = 2.2;
    vec3 color = texture(screen, vs_texcoord).rgb;
    vec4 color4 = vec4(color, 1.0);
    FragColor.rgb = pow(color4.rgb, vec3(1.0/gamma));
}