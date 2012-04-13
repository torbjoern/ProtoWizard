#version 330

uniform vec4 vColor;
uniform mat4 projMatrix;
uniform mat4 modelviewMatrix;

layout(location = 0) in vec4 position;

out vec4 fColor;

void main()
{
	fColor = vColor;
	gl_Position = projMatrix * modelviewMatrix * vec4( position.x, position.y, 0.0, 1.0 );
}

