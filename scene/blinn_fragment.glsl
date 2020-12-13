#version 330 core

in vec3 normal;
in vec3 fragPos;
out vec4 color;

struct Material 
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
struct Light 
{
	vec3 pos;
	vec3 color;
	float constant;
	float linear;
	float quadratic;
};

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

void main() 
{
	vec3 amb = 0.2 * light.color * material.ambient;
	float diff = max(dot(normalize(normal), normalize(light.pos - fragPos)), 0.0);
	vec3 diffuse = diff * light.color * material.diffuse;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = normalize(reflect(fragPos - light.pos, normal));
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = spec * light.color;
	color = vec4((diffuse + amb + specular), 1.0f);
}