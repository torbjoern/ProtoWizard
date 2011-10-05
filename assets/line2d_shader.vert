#version 330

uniform mat4 mvp;

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 in_color;

out vec4 fColor;

void main()
{
	fColor = in_color;
	gl_Position = mvp * vec4( position.x, position.y, 0.0, 1.0 );
}

