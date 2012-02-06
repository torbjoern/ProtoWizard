#version 330


out vec4 outputColor;

uniform vec4 diffuseColor;
uniform float shininess;

in vec3 fNormal, lightDir, eyeVec;
in vec2 fTexcoord;

uniform sampler2D tex0;
uniform int use_textures;

void main (void)
{
	float alpha = diffuseColor.a;
	
	vec4 base_diffuse;
	if ( use_textures == 1 )
	{
		base_diffuse = diffuseColor * texture2D(tex0,fTexcoord.st);
	} else {
		base_diffuse = diffuseColor;
	}
							
	vec3 N = normalize(fNormal);
	vec3 L = normalize(lightDir);
	//vec3 L = normalize( vec3( 0.4, 0.4, 0.4 ) );
	
	vec4 final_color = base_diffuse * 0.5;
	
	float lambertTerm = dot(N,L);
	
	if(lambertTerm > 0.0)
	{
			
		
		vec3 E = normalize(eyeVec);
		vec3 R = reflect(-L, N);
		
		float specular = pow( max(dot(R, E), 0.0), shininess );
		
		
		final_color += (base_diffuse * lambertTerm) + specular;
	}

	
	//final_color = vec4(0.5 + 0.5 * fNormal, 0.0);
	
	outputColor = vec4(final_color.rgb, alpha);	
}