#ifndef _VERTEX_TYPES_H
#define _VERTEX_TYPES_H

#define GLM_SWIZZLE 
#include <glm/glm.hpp>

// some gpu's like vertex layout in multiples of 32 bytes
// http://www.opengl.org/wiki/Vertex_Formats

struct LineSegmentState
{
	float x1, y1;
	glm::vec4 color1;
	float x2, y2;
	glm::vec4 color2;
};

struct TexturedVertex
{
	float x,y;
	float u,v;
	
	TexturedVertex(){}
	
	TexturedVertex(float xx,float yy,float uu, float vv){x = xx; y = yy; u = uu; v = vv;}
	
	void set(float xx,float yy,float uu, float vv){x = xx; y = yy; u = uu; v = vv;}
};

struct XYZVertex
{
	float x,y,z;

	XYZVertex(){}

	XYZVertex(const glm::vec3& v){ x = v.x; y = v.y; z = v.z; }
};

struct PNVertex
{
	float x,y,z;
	float nx, ny, nz;

	explicit PNVertex(const glm::vec3& v, const glm::vec3& n)
	{ 
		x = v.x; y = v.y; z = v.z;
		nx = n.x; ny = n.y; nz = n.z;
	}
};

struct Vertex_VNT
{
	float x,y,z;
	float nx, ny, nz;
	float s, t;

	Vertex_VNT(){}

	Vertex_VNT(const glm::vec3& vert_coord, const glm::vec3& normal, const glm::vec2& tex_coord){ 
		x = vert_coord.x; y = vert_coord.y; z = vert_coord.z; 
		nx = normal.x; ny = normal.y; nz = normal.z; 
		s = tex_coord.s; t = tex_coord.t; 
	}
};

struct Vertex_VNC
{
	float x,y,z;
	float nx, ny, nz;
	float r,g,b;

	Vertex_VNC(){}

	Vertex_VNC(const glm::vec3& vert_coord, const glm::vec3& normal, const glm::vec3& color){ 
		x = vert_coord.x; y = vert_coord.y; z = vert_coord.z; 
		nx = normal.x; ny = normal.y; nz = normal.z; 
		r = color.r; g = color.g; b = color.b; 
	}

	static const int stride = 3*4;
};

#endif

