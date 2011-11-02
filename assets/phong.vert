#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 modelviewMatrix;
uniform mat4 projMatrix;
uniform vec3 cameraSpaceLightPos;

out vec3 fNormal, lightDir, eyeVec;


void main()
{	
	fNormal = vec3( modelviewMatrix * vec4(normal,0.0) );

	vec3 vVertex = vec3( modelviewMatrix * vec4(position,1.0) );

	lightDir = vec3(cameraSpaceLightPos - vVertex);
	eyeVec = -vVertex;


	
	gl_Position = projMatrix * vec4(vVertex,1.0);	
}


