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
uniform vec3 floorColor;

vec3 ambientLighting(vec3 normal, vec3 fragPos, Light light) {

    vec3 view_dir = normalize(camera - fragPos);
    vec3 light_dir = normalize(light.position - fragPos);
    vec3 reflect_dir = reflect(light_dir, vs_normal);
    vec3 half_dir = normalize(light_dir + view_dir);

    float ndotl = (dot(light_dir, normal) + 1.0) * 0.5;

    vec3 diffuse = (ndotl * light.color) * 1.5; // Making this less intense
    
    vec3 light_color;
    
    if (ndotl > 0.5) {
        light_color = pal.lit * diffuse;
    }
    else {
        light_color = pal.unlit * diffuse;
    }

    return light_color;
}

void main()
{
    vec3 realNormal = normalize(vs_normal);

    if (realNormal.g > 0.9) {
        FragColor = vec4(floorColor, 1.0);
        return;
    }

    vec3 lighting = ambientLighting(realNormal, vs_position, light);
    vec3 final_color = lighting;
    
    FragColor = vec4(final_color, 1.0);
}