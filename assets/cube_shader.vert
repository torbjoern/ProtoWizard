#version 330

uniform vec4 vColor;
// TODO send in final proj*model matrix? but then one can't calc normal in modelspace..?
uniform mat4 projMatrix;
uniform mat4 modelviewMatrix;
uniform vec3 translate;
uniform float radius;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec4 fColor;
out vec3 fragmentNormal;



void main()
{
	fColor = vColor;
	
	fragmentNormal = (modelviewMatrix*vec4(normal, 0.0)).xyz;
	gl_Position = projMatrix * modelviewMatrix * vec4( translate + radius * position, 1.0 );
	
}

