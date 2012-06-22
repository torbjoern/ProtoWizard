#version 150

#extension GL_EXT_geometry_shader4 : enable


layout(triangles) in; // points/lines/LINES_ADJACENCY/TRIANGLES/TRIANGLES_ADJACENCY
layout(line_strip) out; //  points, line_strip, and triangle_strip
layout(max_vertices = 8) out;



uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in mat4 mvp[];
in mat4 normal_mtx[];
in vec3 fNormal[];

out vec4 fColor;

void main (void)
{
   mat4 m = mvp[0];
   float sc = 1.0 / 10.0;
	 vec4 startColor = fColor = vec4(1,0,0,0);
	 vec4 endColor = fColor = vec4(1,1,0,0);
   gl_FrontColor = vec4(1,1,1,1);

	#if 1
	// Wireframe
	for(int i=0;i<gl_VerticesIn+1;i++)
	{
		 fColor = vec4(1,1,1,1);
		 gl_Position=m*gl_PositionIn[i % gl_VerticesIn];  fColor = vec4(0,1,0,1); EmitVertex();
	}
	EndPrimitive();
	#endif

	#if 0
	// Per vertex normals
	for(int i=0;i<gl_VerticesIn;i++)
	{
		 gl_Position=m*gl_PositionIn[i]; fColor = startColor; EmitVertex();
		 
		 vec4 pn = vec4( gl_PositionIn[i].xyz + sc*fNormal[i], 1.0);
		 gl_Position = m* pn; fColor = endColor; EmitVertex();
		 EndPrimitive();
	}
	#endif

	// Per face normals
	#if 1
		vec4 cent = (gl_PositionIn[0] + gl_PositionIn[1] + gl_PositionIn[2]) / 3.0;
		vec3 avg_face_normal = normalize( (fNormal[0] + fNormal[1] + fNormal[2]) / 3.0 );
		
		gl_Position = m*cent; fColor = startColor; EmitVertex();    
		vec4 pn = vec4(cent.xyz + sc*avg_face_normal, 1.0);
		gl_Position = m*pn; fColor = endColor; EmitVertex();
	 EndPrimitive();
	#endif
}