#version 430
in vec3 v2fNormal;
in vec3 v2fPos;
in vec3 v2fView;
in vec3 uAmbient;
in vec3 uDiffuse;
in vec3 uSpecular;
in float uShininess;
in float uAlpha;
in vec2 v2fTexCoord;
in float oTex;

//layout( location = 2 ) uniform vec3 uLightDir; // should be normalized! kuLightDirk = 1
layout( location = 0 ) out vec4 oColor;

float specularStrength = 0.5;
vec3 result;
vec3 emissive;

layout( binding = 0 ) uniform sampler2D uTexture;

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 5
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 v2fPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - v2fPos);
	float diff = max( 0.0, dot(normal, lightDir ) );
    //vec3 reflectDir = reflect(normal,-lightDir);
    vec3 halfDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfDir), 0.0), uShininess);
    float distance    = length(light.position - v2fPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
  			     light.quadratic * (distance * distance));


	vec3 ambient = light.ambient * uAmbient;
	vec3 diffuse = light.diffuse * (diff * uDiffuse);
	vec3 specular = light.specular * (uSpecular * spec);
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

void main()
{
    vec3 result = {0.f,0.f,0.f};
	vec3 normal = normalize(v2fNormal);
	vec3 viewDir = normalize(v2fView - v2fPos);

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], normal, v2fPos, viewDir);

    if(oTex == 1.f){
	   // oColor = vec4(result * texture( uTexture, v2fTexCoord ).rgb, uAlpha);
       oColor = texture( uTexture, v2fTexCoord) * vec4(result, 1.f);
	}
    else
        oColor = vec4(result, uAlpha);



}
