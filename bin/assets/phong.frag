#version 330


out vec4 outputColor;

uniform vec4 diffuseColor;

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
		final_color += (base_diffuse * lambertTerm);
	}

	
	//final_color = vec4(0.5 + 0.5 * fNormal, 0.0);
	
	/*
	
	if ( gl_FrontFacing )
		final_color.xyz *= vLightFront;
	else
		final_color.xyz *= vLightBack;
		*/
		
	outputColor = vec4(final_color.rgb, alpha);	
}