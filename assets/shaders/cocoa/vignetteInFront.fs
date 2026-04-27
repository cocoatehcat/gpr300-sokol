#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// Uniforms
//uniform sampler2D screen;  
uniform vec3 backcolor;
uniform sampler2D sceneTexture;
uniform sampler2D stencilTexture;

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
    vec3 sceneColor = texture(sceneTexture, vs_texcoord).rgb;
    float stencilValue = texture(stencilTexture, vs_texcoord).r;

    if (stencilValue > 0.5) {
        FragColor = vec4(sceneColor, 1.0);
        return;
    }

    vec3 color = sceneColor;
    color *= vec3(vig); // Changing this is cool
    FragColor = vec4(color, 1.0);
}