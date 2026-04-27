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
    // Center UVs and correct aspect ratio
    uv *= 1.0 - uv.yx;

    // Define vignette intensity from center of screen
    float vig = uv.x * uv.y * 16.0;
    vig = pow(vig, 0.25);

    // Adding effect to background
    vec3 color = backcolor;
    color *= vec3(vig); // Changing this is cool
    FragColor = vec4(color, 1.0);
}