#version 430
in vec3 v2fColor;
in vec3 v2fNormal;
in vec3 v2fPos;
in vec3 v2fView;
in vec2 v2fTexCoord;
in float v2fTexBool;

layout( location = 0 ) out vec3 oColor;
layout( binding = 0 ) uniform sampler2D uTexture;

float specularStrength = 0.5;


struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

#define NR_POINT_LIGHTS 3 
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 v2fPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - v2fPos);
	float diff = max( 0.0, dot(normal, lightDir ) );
    vec3 reflectDir = reflect(normal,-lightDir);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    float distance    = length(light.position - v2fPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    


	vec3 ambient = light.ambient * material.ambient;
	vec3 diffuse = light.diffuse * diff * material.diffuse;
	vec3 specular = light.specular * material.specular * spec;  
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular) * v2fColor;
} 

void main()
{
	vec3 normal = normalize(v2fNormal);
	vec3 viewDir = normalize(v2fView - v2fPos);
	
    vec3 result;
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], normal, v2fPos, viewDir);
    
    if (v2fTexBool == 1.f)
        oColor = texture( uTexture, v2fTexCoord ).rgb;
    else
        oColor = result;
}
