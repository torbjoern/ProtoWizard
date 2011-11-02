#version 330


out vec4 outputColor;

uniform vec4 diffuseColor;
uniform vec4 lightIntensity;
uniform vec4 ambientIntensity;
uniform vec3 cameraSpaceLightPos;
uniform float lightAttenuation;
uniform float shininess;



in vec3 fNormal, lightDir, eyeVec;


void main (void)
{
	float alpha = diffuseColor.a;
	vec4 final_color = diffuseColor + ambientIntensity;
							
	vec3 N = normalize(fNormal);
	vec3 L = normalize(lightDir);
	
	float lambertTerm = dot(N,L);
	
	if(lambertTerm > 0.0)
	{
		final_color += (diffuseColor * lambertTerm) ;	
		
		vec3 E = normalize(eyeVec);
		vec3 R = reflect(-L, N);
		float specular = pow( max(dot(R, E), 0.0), 
		                 shininess );
		final_color += specular;	
	}
	


	outputColor = vec4(final_color.xyz, alpha);			
}