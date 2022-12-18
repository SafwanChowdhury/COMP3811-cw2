#version 430
in vec3 v2fColor;
in vec3 v2fNormal;
in vec3 v2fPos;
in vec3 v2fView;

//layout( location = 2 ) uniform vec3 uLightDir; // should be normalized! kuLightDirk = 1
uniform vec3 uLightPos;
layout( location = 3 ) uniform vec3 uLightColor;

layout( location = 0 ) out vec3 oColor;
float specularStrength = 0.5;


struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;  

void main()
{
	vec3 ambient = light.ambient * material.ambient;

	vec3 lightDir = normalize(uLightPos - v2fPos);
	vec3 normal = normalize(v2fNormal);
	float nDotL = max( 0.0, dot(normal, lightDir ) );
	vec3 diffuse = light.diffuse * (nDotL * material.diffuse);

	vec3 viewDir = normalize(v2fView - v2fPos);
	vec3 reflectDir = reflect(normal,-lightDir);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (material.specular * spec);  


	oColor = (ambient + diffuse + specular) * v2fColor; 
}
