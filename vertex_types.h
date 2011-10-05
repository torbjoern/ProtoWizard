#ifndef _VERTEX_TYPES_H
#define _VERTEX_TYPES_H

struct ColorRGBA
{
	float r,g,b,a;
};

struct LineSegmentState
{
	float x1, y1;
	ColorRGBA color1;
	float x2, y2;
	ColorRGBA color2;
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
	XYZVertex(glm::vec3 v){ x = v.x; y = v.y; z = v.z; }
};

struct PNVertex
{
	float x,y,z;
	float nx, ny, nz;
	explicit PNVertex(glm::vec3 v, glm::vec3 n)
	{ 
		x = v.x; y = v.y; z = v.z;
		nx = n.x; ny = n.y; nz = n.z;
	}
};

#endif

