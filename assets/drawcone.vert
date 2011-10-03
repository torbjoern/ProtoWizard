#version 330

uniform vec4 vColor;
uniform mat4 projMatrix;
uniform mat4 modelviewMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec4 fColor;
out vec3 fNormal;



void main()
{
	fColor = vColor;
	
	fNormal = (modelviewMatrix*vec4(normal, 0.0)).xyz;
	gl_Position = projMatrix * modelviewMatrix * vec4( position, 1.0 );
	
}

