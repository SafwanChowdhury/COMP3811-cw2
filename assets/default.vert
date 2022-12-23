#version 430
layout( location = 0 ) in vec3 iPosition;
layout( location = 1 ) in vec3 iColor;
layout( location = 2 ) in vec3 iNormal;
layout( location = 3 ) in vec2 iTexCoord; 


layout( location = 0 ) uniform mat4 uProjection; 
layout( location = 1 ) uniform mat3 uNormalMatrix; 
layout( location = 4 ) uniform mat4 uCamPos;
layout( location = 5 ) uniform mat4 uModel;
layout( location = 6 ) uniform mat4 uView;
layout( location = 7 ) uniform float isTex;


out vec3 v2fColor;
out vec3 v2fNormal;
out vec3 v2fPos;
out vec3 v2fView;
out vec2 v2fTexCoord;
out float oTex;

void main()
{
	v2fColor = iColor;
	v2fNormal = normalize(uNormalMatrix * iNormal);
	v2fPos = vec3(uModel * vec4(iPosition,1.0));
	v2fView = vec3(uView);
	v2fTexCoord = iTexCoord;
	oTex = isTex;
	gl_Position = (uProjection * uView  * uModel * vec4( iPosition, 1.0 ));

}
