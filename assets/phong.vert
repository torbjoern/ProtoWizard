#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;


uniform int isSphere;

uniform vec3 cameraSpaceLightPos;

out vec3 fNormal, lightDir, eyeVec;



void main()
{	
	// http://www.lighthouse3d.com/tutorials/glsl-tutorial/the-normal-matrix/
	
	if ( isSphere== 1 )
	{
		vec3 sphereNormal = vec3( position );
		fNormal = sphereNormal;
	}else
	{
		fNormal = normal;
	}
	
	
	vec3 vVertex = vec3( worldMatrix * vec4(position,1.0) );

	lightDir = cameraSpaceLightPos - vVertex.xyz;
	eyeVec = -vVertex;


	
	mat4 mvp = projMatrix * viewMatrix * worldMatrix;
	gl_Position =  mvp * vec4(position,1.0);	
}


