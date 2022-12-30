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
in float oEmi;
in float oMulti;
//layout( location = 2 ) uniform vec3 uLightDir; // should be normalized! kuLightDirk = 1
layout( location = 0 ) out vec4 oColor;

vec3 result = {0.f,0.f,0.f};

layout( binding = 0 ) uniform sampler2D uTexture;
layout( binding = 1 ) uniform sampler2D uTexture1;
layout( binding = 2 ) uniform sampler2D uTexture2;
layout( binding = 3 ) uniform sampler2D uTexture3;
layout( binding = 4 ) uniform sampler2D uTexture4;

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


uniform vec3 emissive;


#define NR_POINT_LIGHTS 5
uniform PointLight pointLights[NR_POINT_LIGHTS];


vec3 CalcPointLight(PointLight light, vec3 v2fNormal, vec3 v2fPos, vec3 v2fView)
{
    vec3 normal = normalize(v2fNormal);
	vec3 viewDir = normalize(v2fView - v2fPos);
	vec3 lightDir = normalize(light.position - v2fPos);

	float diff = max( 0.0, dot(normal, lightDir ) );
    float distance    = length(light.position - v2fPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
  			     light.quadratic * (distance * distance));
	vec3 ambient = light.ambient * uAmbient;
	vec3 diffuse = light.diffuse * (diff * uDiffuse);

    ambient  *= attenuation;
    diffuse  *= attenuation;



    vec3 halfDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfDir), 0.0), uShininess);
    vec3 specular = light.specular * (uSpecular * spec);
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

void main()
{

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], v2fNormal, v2fPos, v2fView);

    if(oTex > 0.5f){
	if(oMulti > 0.5f) {
		oColor = (texture( uTexture, v2fTexCoord) * texture( uTexture1, v2fTexCoord) * vec4(result, uAlpha));
	}
	else{
       		oColor = (texture( uTexture, v2fTexCoord) * vec4(result, uAlpha));
	}
	}
    else{
        if (oEmi > 0.5f)
		
            oColor = vec4(emissive.x, emissive.y, emissive.z ,1.f);
        else
            oColor = vec4(result, uAlpha);
     }



}
