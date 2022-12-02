#version 430
layout( location = 0 ) in vec3 iPosition;
layout( location = 1 ) in vec3 iColor;
layout( location = 0 ) uniform mat4 uProjCameraWorld; 

out vec3 v2fColor;


void main()
{
	v2fColor = iColor;
	gl_Position = uProjCameraWorld * vec4( iPosition, 1.0 ); 
}
