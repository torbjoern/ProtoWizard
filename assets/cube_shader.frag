#version 330 

in vec4 fColor;
in vec3 fragmentNormal;
out vec4 out_Color;

void main()
{  
	//float intensity = max(dot(fragmentNormal, vec3(0.0, 0.0, 1.0)), 0.0);
	//out_Color = fColor*intensity;

	//out_Color = fColor;
	out_Color =  vec4( 0.5 + fragmentNormal * 0.5 , 1.0 );
	//out_Color =  vec4( fragmentNormal , 1.0 );
}

