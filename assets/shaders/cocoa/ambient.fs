#version 410

precision mediump float;

out vec4 FragColor;

struct Light {
    vec3 color;
    vec3 position;
};

struct Palette {
    vec3 color1;
    vec3 color2;
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
uniform Material material;
uniform float alpha;
uniform vec3 ambientColor;
uniform sampler2D mainTexture;
uniform sampler2D toonShader;  

vec3 toonshading(vec3 normal, vec3 fragPos, Light light) {
    vec3 view_dir = normalize(camera - fragPos);
    vec3 light_dir = normalize(light.position - fragPos);
    vec3 reflect_dir = reflect(light_dir, vs_normal);
    vec3 half_dir = normalize(light_dir + view_dir);

    float ndotl = (dot(normal, light_dir) + 1.0) * 0.5;
    //float specular = pow(max(dot(normal, half_dir), 0.0), material.shininess);

    vec3 gradient = texture(toonShader, vec2(ndotl, ndotl)).rbg;

    vec3 light_color = mix(pal.color1, pal.color2, gradient);

    // Assign lighting based off material
    //vec3 lightColor = (material.ambient + (material.diffuse * diffuse + material.specular * specular)) * light.color;

    return light_color;
}

void main()
{
    vec3 lighting = toonshading(vs_normal, vs_position, light);
    //vec3 object_color = vs_normal * 0.5 + 0.5;
    vec3 object_color = texture(mainTexture, vs_texcoord).rbg;
    vec3 final_color = lighting;
    
    FragColor = vec4(vs_normal, 1.0);
}