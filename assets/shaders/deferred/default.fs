#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// Uniforms
uniform sampler2D blinnphong;
uniform sampler2D albedo;  

void main()
{
    vec3 color = texture(albedo, vs_texcoord).rgb;
    vec3 lighting = texture(blinnphong, vs_texcoord).rgb;
    FragColor = vec4(lighting * color, 1.0);
}