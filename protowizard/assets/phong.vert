#version 330

//#extension GL_ARB_explicit_attrib_location : enable // or version 330

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
	
	if ( isSphere == 1 )
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


	mat4 mv = viewMatrix * worldMatrix;
	mat4 mvp = projMatrix * mv;
	gl_Position =  mvp * vec4(position,1.0);	
	
    // explaination of why normals must be transformed with a normal matrix: www.lighthouse3d.com/.../the-normal-matrix/
	//mat4 my_NormalMatrix = transpose(inverse(mv)); //modelview inverse;
	//fNormal = normalize(my_NormalMatrix * vec4(fNormal,1.0)).xyz;  
}


