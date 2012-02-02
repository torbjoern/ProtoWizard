#version 330


out vec4 outputColor;

uniform vec4 diffuseColor;
uniform vec3 emissiveColor;
uniform float shininess;

in vec3 fNormal, lightDir, eyeVec;


void main (void)
{
	float alpha = diffuseColor.a;
	
	
							
	vec3 N = normalize(fNormal);
	vec3 L = normalize(lightDir);
	//vec3 L = normalize( vec3( 0.4, 0.4, 0.4 ) );
	

	if ( emissiveColor.r > 0.0 )
	{
		outputColor = vec4(emissiveColor, 1.0);
		return;
	}
	
	
	vec4 final_color = diffuseColor * 0.5;
	
	float lambertTerm = dot(N,L);
	
	if(lambertTerm > 0.0)
	{
			
		
		vec3 E = normalize(eyeVec);
		vec3 R = reflect(-L, N);
		
		float specular = pow( max(dot(R, E), 0.0), shininess );
		
		
		final_color += (diffuseColor * lambertTerm) + specular;
	}

	
	//final_color = vec4(0.5 + 0.5 * fNormal, 0.0);
	
	outputColor = vec4(final_color.rgb, alpha);	
	
}