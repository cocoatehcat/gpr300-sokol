#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// Uniforms
uniform sampler2D screen;  

void main()
{
    vec3 color = texture(screen, vs_texcoord).rgb;

    vec3 red = vec3(0.625, 0.375, 0);
    vec3 green = vec3(0.70, 0.30, 0);
    vec3 blue = vec3(0, 0.30, 0.70);

    float average = 0.4157 * color.r + 0.4627 * color.g + 0.2980 * color.b;

    vec3 rgb = vec3(dot(color.rgb, red), dot(color.rgb, green), dot(color.rgb, blue));
    FragColor = vec4(rgb, 1.0);
}