#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// Uniforms
uniform sampler2D screen;  
uniform float distortion;

vec3 offset = vec3(0.01, 0.005, -0.005);
const vec2 direction = vec2(1.0);

void main()
{
    vec3 color = texture(screen, vs_texcoord).rgb;

    float rChannel = texture(screen, vs_texcoord + (direction * vec2(distortion) * 0.01)).r;
    float gChannel = texture(screen, vs_texcoord + (direction * vec2(distortion) * 0.005)).g;
    float bChannel = texture(screen, vs_texcoord + (direction * vec2(distortion) * -0.005)).b;

    vec3 finalColor = vec3(rChannel, gChannel, bChannel);

    FragColor = vec4(finalColor, 1.0);
}