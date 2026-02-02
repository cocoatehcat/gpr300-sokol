#version 410

precision mediump float;

out vec4 FragColor;

struct Light {
    vec3 color;
    vec3 position;
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
in mat3 TBN;

// Uniforms
uniform vec3 camera;
uniform Light light;
uniform Material material;
uniform float alpha;
uniform vec3 ambientColor;
uniform sampler2D mainTexture;
uniform sampler2D normalMap;  

vec3 blinnphong(vec3 normal, vec3 fragPos, Light light, vec3 texture) {
    vec3 view_dir = normalize(camera - fragPos);
    vec3 light_dir = normalize(light.position - fragPos);
    vec3 reflect_dir = reflect(-light_dir, vs_normal);
    vec3 half_dir = normalize(light_dir + view_dir);

    float diffuse = max(dot(light_dir, normal), 0.0);
    float specular = pow(max(dot(normal, half_dir), 0.0), material.shininess);

    // Assign lighting based off material
    vec3 lightColor = (material.ambient + (material.diffuse * diffuse * texture + material.specular * specular)) * light.color;

    return lightColor;

    // dot product
    // glsl: dot(vec3, vec3);
    //float pdl = dot(fragPos, lightPos); // position dot product light
    //return normalize(vec3(pdl) * lightColor);
}

void main()
{
    vec3 texture_color = texture(mainTexture, vs_texcoord).rbg;
    vec3 normal_color = texture(normalMap, vs_texcoord).rbg;
    normal_color = normalize(normal_color * 2.0 - 1.0);
    vec3 lighting = blinnphong(normal_color, vs_position, light, texture_color) + (alpha * ambientColor * texture_color);
    vec3 object_color = vs_normal * 0.5 + 0.5;
    vec3 final_color = object_color * lighting;
    
    FragColor = vec4(final_color, 1.0);
}