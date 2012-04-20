#version 150

#extension GL_ARB_explicit_attrib_location : enable // or #version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform vec3 emissiveColor;
uniform vec4 diffuseColor;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 fNormal;
out mat4 mvp;
out mat4 normal_mtx;

void main()
{	
	fNormal = normal;

	mat4 modelview = viewMatrix * worldMatrix;
	mvp = projMatrix * modelview;
	
	// http://www.lighthouse3d.com/tutorials/glsl-tutorial/the-normal-matrix/
	normal_mtx = transpose( inverse(modelview) );
	gl_Position =  vec4(position,1.0);	
}


