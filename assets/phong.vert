#version 330


struct gl_LightSourceParameters {
		vec4 ambient;
		vec4 diffuse;
		vec4 specular;
		vec4 position;
		vec4 halfVector;
		//vec3 spotDirection;
		//float spotExponent;
		//float spotCutoff; // (range: [0.0,90.0], 180.0)
		//float spotCosCutoff; // (range: [1.0,0.0],-1.0)
		float constantAttenuation;
		float linearAttenuation;
		float quadraticAttenuation;
	};
	
struct gl_MaterialParameters {
		vec4 emission;
		vec4 ambient;
		vec4 diffuse;
		vec4 specular;
		float shininess;
	};
	
#define gl_MaxLights 1
uniform gl_LightSourceParameters gl_LightSource[gl_MaxLights];	

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


