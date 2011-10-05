#version 330 

in float fIntensity;
in vec4 fColor;
in vec3 fragmentNormal;
in vec3 to_light;
out vec4 out_Color;


void main()
{  
	vec3 nl = normalize(to_light);
	float intensity = max(dot(fragmentNormal, nl), 0.0) ;
	
	vec4 normalColor = vec4( 0.5 + fragmentNormal * 0.5 , 1.0 );
	out_Color = normalColor * fIntensity;
	//out_Color = fColor*fIntensity;
	//out_Color = fColor*intensity;
}

