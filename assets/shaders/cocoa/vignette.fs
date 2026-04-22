#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// Uniforms
uniform sampler2D screen;  

void main()
{
    //uv *=  1.0 - uv.yx;   //vec2(1.0)- uv.yx; -> 1.-u.yx; Thanks FabriceNeyret !
    //float vig = uv.x*uv.y * 15.0; // multiply with sth for intensity
    //vig = pow(vig, 0.25); // change pow for modifying the extend of the  vignette
    //vec3 color = vec3(0.0);

    vec2 uv = vs_texcoord;
    uv *= 1.0 - uv.yx;

    float vig = uv.x * uv.y * 16.0;
    vig = pow(vig, 0.25);

    vec3 color = texture(screen, vs_texcoord).rgb;
    color *= vec3(vig);
    FragColor = vec4(color, 1.0);
}