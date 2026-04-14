#version 410

struct Light {
    vec3 color;
    vec3 position;
    float radius;
};

in vec2 vs_texcoord;

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;
uniform sampler2D g_material;

uniform Light light;
uniform vec3 camera_position;

out vec4 FragLighting;

vec3 blinnphong(vec3 position, vec3 normal, vec4 material) {
    vec3 view_dir = normalize(camera_position - position);
    vec3 light_dir = normalize(light.position - position);
    vec3 half_dir = normalize(light_dir + view_dir);

    float diffuse = max(dot(normal, light_dir), 0.0);
    float specular = pow(max(dot(normal, half_dir), 0.0), material.a);

    vec3 diffuseV = diffuse * vec3(material.g);
    vec3 specularV = pow(specular, material.a * 128.0) * vec3(material.b);

    return (diffuseV + specularV) * light.color;
}

void main() 
{
    vec2 uv = gl_FragCoord.xy / vec2(800, 600);

    vec3 albedo = texture(g_albedo, uv).rgb;
    vec3 normal = texture(g_normal, uv).rgb;
    vec3 worldPos = texture(g_position, uv).rgb;
    vec4 mat = texture(g_material, uv).rgba;

    vec3 lightCol = blinnphong(worldPos, normal, mat);

    FragLighting = vec4(albedo * lightCol, 1.0);
}
