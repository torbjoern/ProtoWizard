#version 330


out vec4 outputColor;

uniform vec4 diffuseColor;
uniform vec3 emissiveColor;
uniform vec4 lightIntensity;
uniform vec4 ambientIntensity;
uniform vec3 cameraSpaceLightPos;
uniform float lightAttenuation;
uniform float shininess;

uniform sampler2D tex0;
uniform int use_textures;


in vec3 fNormal, lightDir, eyeVec;

in vec4 fColor;

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
	
	
	vec4 final_color = diffuseColor * 0.5 + ambientIntensity;
	
	float lambertTerm = dot(N,L);
	
	if(lambertTerm > 0.0)
	{
			
		
		vec3 E = normalize(eyeVec);
		vec3 R = reflect(-L, N);
		
		float specular = pow( max(dot(R, E), 0.0), shininess );
		
		
		final_color += (diffuseColor * lambertTerm) + specular;
	}

	if (use_textures == 1) {
		final_color = mix( texture2D(tex0, vec2(0.0, 0.0)), final_color, 0.9 ) ;
	}
	
	
	//final_color = vec4(0.5 + 0.5 * fNormal, 0.0);
	
	outputColor = vec4(fColor.rgb, alpha);	
	
}