#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

// Uniforms
uniform vec3 camera;
uniform sampler2D waveSpec; 
uniform sampler2D waveTex;
uniform sampler2D waveWarp;  
uniform vec3 waterColor;
uniform float warpScale;
uniform float albedoScale;
uniform float specScale;
uniform float upper;
uniform float lower;

uniform float time;

void main()
{
    vec2 dir = vec2(1, 0);
    vec2 uv = vs_texcoord + vec2(time * dir);
    uv.x += 0.01 * sin(uv.x * 3.5 + time);
    uv.y += -0.35 * sin(uv.y * 1.5 + time);

    // Warp
    vec2 warpUV = vs_texcoord * warpScale;
    vec2 warpScroll = vec2(0.5, 0.5) * time;
    vec2 warp = texture(waveWarp, warpUV + warpScroll).xy;

    // Albedo
    vec2 albedoUV = vs_texcoord * albedoScale;
    vec4 albedo = texture(waveTex, albedoUV + warp).rgba;

    vec3 finalColor = waterColor + vec3(albedo.a);

    // Spec
    vec2 specUV = vs_texcoord * specScale;
    vec2 specScroll = vec2(0.5, 0.5) * time;
    //vec3 spec = texture(waveSpec, specUV + specScroll).rgb;

    vec4 sample1 = texture(waveSpec, specUV + vec2(0.5, 0.5) * time);
    vec4 sample2 = texture(waveSpec, specUV + vec2(-0.5, -0.5) * time);
    vec3 spec = vec3(sample1 + sample2);

    vec3 specColor = vec3((sample1 * 0.75) - (sample2 * 0.25));

    //fresnal effect!
    float fresnal = dot(normalize(camera), vec3(0.0, 1.0, 0.0));

    const vec3 kBright = vec3(0.299, 0.587, 0.114);
    float brightness = dot(spec, kBright);

    if (brightness <= lower || brightness >= upper) {
        finalColor = mix(finalColor, finalColor + spec, fresnal);
    }
    
    FragColor = vec4(finalColor, 1.0);
}