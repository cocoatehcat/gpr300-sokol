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

// Uniforms
uniform vec3 camera;
uniform Light light;
uniform Material material;
uniform float alpha;
uniform vec3 ambientColor;

vec3 blinnphong(vec3 normal, vec3 fragPos, Light light) {
    vec3 view_dir = normalize(camera - fragPos);
    vec3 light_dir = normalize(light.position - fragPos);
    vec3 reflect_dir = reflect(light_dir, vs_normal);
    vec3 half_dir = normalize(light_dir + view_dir);

    float diffuse = max(dot(normal, light_dir), 0.0);
    float specular = pow(max(dot(normal, half_dir), 0.0), material.shininess);

    // Assign lighting based off material
    vec3 lightColor = (material.ambient + (material.diffuse * diffuse + material.specular * specular)) * light.color;

    return lightColor;

    // dot product
    // glsl: dot(vec3, vec3);
    //float pdl = dot(fragPos, lightPos); // position dot product light
    //return normalize(vec3(pdl) * lightColor);
}

void main()
{
  vec3 lighting = blinnphong(vs_normal, vs_position, light) + (alpha * ambientColor);
  vec3 object_color = vs_normal * 0.5 + 0.5;
  vec3 final_color = object_color * lighting;

  FragColor = vec4(final_color, 1.0);
}