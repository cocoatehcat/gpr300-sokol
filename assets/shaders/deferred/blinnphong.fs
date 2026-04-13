#version 410

struct Light {
    vec3 color;
    vec3 position;
};

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;
uniform sampler2D g_material;

uniform Light light;
uniform vec3 camera_position;

out vec4 FragLighting;

void main() 
{
    //vec3 albedo = texture(g_albedo, UV).xyz;

    FragLighting = vec4(light.color, 1.0);
}
