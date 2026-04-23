#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// Uniforms
//uniform sampler2D screen;  
uniform vec3 backcolor;

void main()
{
    // Referencing Shadertoy : https://www.shadertoy.com/view/lsKSWR
    vec2 uv = vs_texcoord;
    uv *= 1.0 - uv.yx;

    float vig = uv.x * uv.y * 16.0;
    vig = pow(vig, 0.25);
    //vec3 color = texture(screen, vs_texcoord).rgb;

    vec3 color = backcolor;
    color *= vec3(vig); // Changing this is cool, adding effect
    FragColor = vec4(color, 1.0);
}