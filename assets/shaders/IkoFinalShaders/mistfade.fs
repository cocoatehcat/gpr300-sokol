#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// Uniforms
uniform sampler2D screen;
uniform sampler2D mist_effect;  

void main()
{
    vec2 UV = gl_FragCoord.xy / vec2(800,600);
    vec3 mistValue = texture(mist_effect, vs_texcoord.xy).rgb;
    vec3 color = texture(screen, vs_texcoord.xy).rgb;
    vec3 finalColor = color * mistValue;
    FragColor = vec4(mistValue,1.0f);
}