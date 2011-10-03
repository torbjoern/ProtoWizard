#version 330 

in vec4 fColor;
in vec3 fNormal;

out vec4 out_Color;

void main()
{  
	float intensity;
	
	intensity = max(dot(fNormal, vec3(0.0, 0.0, 1.0)), 0.0);
	
	//out_Color = fColor*intensity;

   out_Color = vec4( (fNormal.xyz+1)*0.5  , 0.5);	
   //out_Color = vec4( 1.0 , 0.0, 0.0, 1.0 );
   //out_Color = fColor;
}

