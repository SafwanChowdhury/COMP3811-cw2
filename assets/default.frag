#version 430
in vec3 v2fColor;
in vec3 v2fNormal;
in vec3 v2fPos;
in vec3 v2fView;

//layout( location = 2 ) uniform vec3 uLightDir; // should be normalized! kuLightDirk = 1
uniform vec3 uLightPos;
layout( location = 3 ) uniform vec3 uLightDiffuse;
layout( location = 4 ) uniform vec3 uSceneAmbient; 


layout( location = 0 ) out vec3 oColor;
float specularStrength = 0.5;

void main()
{
	vec3 viewDir = normalize(v2fView - v2fPos);
	vec3 lightDir = normalize(uLightPos - v2fPos);
	vec3 normal = normalize(v2fNormal);
	vec3 reflectDir = reflect(normal, -lightDir);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * v2fColor;  
	float nDotL = max( 0.0, dot(normal, lightDir ) );
	oColor = (uSceneAmbient + (nDotL + specular) * uLightDiffuse) * v2fColor; 
}
