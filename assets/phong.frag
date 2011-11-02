#version 330


out vec4 outputColor;

uniform vec4 diffuseColor;
uniform vec3 emissiveColor;
uniform vec4 lightIntensity;
uniform vec4 ambientIntensity;
uniform vec3 cameraSpaceLightPos;
uniform float lightAttenuation;
uniform float shininess;



in vec3 fNormal, lightDir, eyeVec;


void main (void)
{
	float alpha = diffuseColor.a;
	
							
	vec3 N = normalize(fNormal);
	vec3 L = normalize(lightDir);
	
	//#define RUBBISH
	#ifdef RUBBISH
	float lambertTerm = clamp( dot(N,L), 0.0, 1.0 ) ;
	lambertTerm /= 0.05 * length( lightDir );
	lambertTerm = clamp( lambertTerm, 0.0, 1.0 );
	vec4 final_color = (ambientIntensity + diffuseColor) * lambertTerm;
	
	#else
	
	if ( emissiveColor.r > 0.0 )
	{
		outputColor = vec4(emissiveColor, 1.0);
		return;
	}
	
	
	vec4 final_color = ambientIntensity;
	
	float lambertTerm = dot(N,L);
	
	if(lambertTerm > 0.0)
	{
		lambertTerm = min( lambertTerm, 1.0 );
		lambertTerm /= 0.1 * pow( length( lightDir ), 1.2 );
		final_color += (diffuseColor * lambertTerm) ;	
		
		vec3 E = normalize(eyeVec);
		vec3 R = reflect(-L, N);
		float specular = pow( max(dot(R, E), 0.0), 
		                 shininess );
		final_color += specular;
	}
	#endif
	
	//final_color = vec4(0.5 + 0.5 * fNormal, 0.0);
	
	outputColor = vec4(final_color.rgb, alpha);			
}