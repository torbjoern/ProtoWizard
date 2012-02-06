#version 330


out vec4 outputColor;

uniform vec4 diffuseColor;
uniform float shininess;

in vec3 fNormal, lightDir, eyeVec;
in vec2 fTexcoord;

uniform sampler2D tex0;

void main (void)
{
	float alpha = diffuseColor.a;
	
							
	vec3 N = normalize(fNormal);
	vec3 L = normalize(lightDir);
	//vec3 L = normalize( vec3( 0.4, 0.4, 0.4 ) );
	
	vec4 diffuse2 = diffuseColor * texture2D(tex0,fTexcoord.st);
	vec4 final_color = diffuse2 * 0.5;
	
	float lambertTerm = dot(N,L);
	
	if(lambertTerm > 0.0)
	{
			
		
		vec3 E = normalize(eyeVec);
		vec3 R = reflect(-L, N);
		
		float specular = pow( max(dot(R, E), 0.0), shininess );
		
		
		final_color += (diffuse2 * lambertTerm) + specular;
	}

	
	//final_color = vec4(0.5 + 0.5 * fNormal, 0.0);
	
	outputColor = vec4(final_color.rgb, alpha);	
	//outputColor = vec4(fTexcoord.s, fTexcoord.t, 0.f, alpha);	
	//outputColor = mix(final_color, texture2D(tex0,fTexcoord.st), 0.5 );
}