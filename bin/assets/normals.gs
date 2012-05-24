#version 150

#extension GL_EXT_geometry_shader4 : enable


layout(triangles) in; // points/lines/LINES_ADJACENCY/TRIANGLES/TRIANGLES_ADJACENCY
layout(line_strip) out; //  points, line_strip, and triangle_strip
layout(max_vertices = 6) out;



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

   float sc = 1.0 / 2.0;

   gl_FrontColor = vec4(1,1,1,1);


/*
// 
for(int i=0;i<gl_VerticesIn + 1;i++)
{
   fColor = vec4(1,1,1,1);
   gl_Position=m*gl_PositionIn[i % gl_VerticesIn];  fColor = vec4(0,1,0,1); EmitVertex();
}
EndPrimitive();
*/


mat4 modelview = viewMatrix * worldMatrix;

for(int i=0;i<gl_VerticesIn;i++)
{
   gl_Position=m*gl_PositionIn[i];  fColor = vec4(1,1,1,1); EmitVertex();
   vec4 pn = modelview * gl_PositionIn[i] + normalize(normal_mtx[i]*(vec4( fNormal[i], 0.0) )) * sc;
   gl_Position=projMatrix * pn;  fColor = vec4(1,1,1,1);  EmitVertex();
   EndPrimitive();
}






   // vec4 cent = (gl_PositionIn[0] + gl_PositionIn[1] + gl_PositionIn[2]) / 3.0;
   // vec3 avg_face_normal = (fNormal[0] + fNormal[1] + fNormal[2]) / 3.0;
   // gl_Position = m*cent; fColor = vec4(1,1,1,1); EmitVertex();    
   // gl_Position=m*(cent + vec4(avg_face_normal * sc,0.0) );  fColor = vec4(0); EmitVertex();


   EndPrimitive();
}