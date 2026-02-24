#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// Uniforms
uniform sampler2D screen;  

// Reference
// http://web.archive.org/web/20081014161121/http://www.colorjack.com/labs/colormatrix/

void main()
{
    vec3 color = texture(screen, vs_texcoord).rgb;
    vec3 red = vec3(0.95, 0.05, 0);
    vec3 green = vec3(0, 0.433, 0.5667);
    vec3 blue = vec3(0, 0.475, 0.525);

    vec3 rgb = vec3(dot(color.rgb, red), dot(color.rgb, green), dot(color.rgb, blue));
    FragColor = vec4(rgb, 1.0);
}