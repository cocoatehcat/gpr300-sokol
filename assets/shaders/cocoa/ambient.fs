#version 410

precision mediump float;

out vec4 FragColor;

struct Light {
    vec3 color;
    vec3 position;
};

struct Palette {
    vec3 lit;
    vec3 unlit;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

// Uniforms
uniform vec3 camera;
uniform Light light;
uniform Palette pal;
uniform vec3 ambientColor;
uniform sampler2D toonShader;  

vec3 ambientLighting(vec3 normal, vec3 fragPos, Light light) {

    vec3 view_dir = normalize(camera - fragPos);
    vec3 light_dir = normalize(light.position - fragPos);
    vec3 reflect_dir = reflect(light_dir, vs_normal);
    vec3 half_dir = normalize(light_dir + view_dir);

    float ndotl = (dot(light_dir, normal) + 1.0) * 0.5;
    //float ndotl = max(dot(normal, light_dir), 0.0);

    //vec3 gradient = texture(toonShader, vec2(ndotl, ndotl)).rgb;
    //vec3 light_color = mix(pal.lit, pal.unlit, gradient);
    
    vec3 light_color;
    if (ndotl > 0.5) {
        light_color = pal.lit;
    }
    else {
        light_color = pal.unlit;
    }

    return light_color;
}

void main()
{
    if (vs_normal.g > 0.93) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    }
    vec3 lighting = ambientLighting(vs_normal, vs_position, light);
    vec3 final_color = lighting;
    
    FragColor = vec4(final_color, 1.0);
}