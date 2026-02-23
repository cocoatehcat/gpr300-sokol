#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// Uniforms
uniform sampler2D screen;  

float distortion = 0.75;

vec2 Pincushion(vec2 uv, float strength) {
    vec2 st = uv - 0.5;
    float uvA = atan(st.x, st.y);
    float uvD = dot(st, st);
    return 0.5 + vec2(sin(uvA), cos(uvA) * sqrt(uvD) * (1.0 - strength * uvD));
}

void main()
{
    vec3 color = texture(screen, vs_texcoord).rgb;

    float rChannel = texture(screen, Pincushion(vs_texcoord, 0.3 * distortion)).r;
    float gChannel = texture(screen, Pincushion(vs_texcoord, 0.15 * distortion)).g;
    float bChannel = texture(screen, Pincushion(vs_texcoord, 0.075 * distortion)).b;

    vec3 finalColor = vec3(rChannel, gChannel, bChannel);

    FragColor = vec4(finalColor, 1.0);
}