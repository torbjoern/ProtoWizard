#pragma once
#define GLM_SWIZZLE 
#include <glm/glm.hpp>

// some gpu's like vertex layout in multiples of 32 bytes
// http://www.opengl.org/wiki/Vertex_Formats

namespace protowizard{
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
		glm::vec3 v;

		XYZVertex(){}

		XYZVertex(const glm::vec3& v) : v(v)
		{
		}
	};

	struct PNVertex
	{
		glm::vec3 v;
		glm::vec3 n;

		explicit PNVertex(const glm::vec3& v, const glm::vec3& n) : v(v), n(n)
		{ 
		}
	};

	struct Vertex_VNT
	{
		glm::vec3 v;
		glm::vec3 n;
		glm::vec2 t;

		Vertex_VNT(){}

		Vertex_VNT(const glm::vec3& v, const glm::vec3& n, const glm::vec2& t) :
			v(v), n(n), t(t)
		{ 
		}
	};

	struct Vertex_VNC
	{
		glm::vec3 v;
		glm::vec3 n;
		glm::vec2 c;

		Vertex_VNC(){}

		Vertex_VNC(const glm::vec3& v, const glm::vec3& n, const glm::vec3& c) :
			v(v), n(n), c(c)
		{ 
		}

		static const int stride = 3*4;
	};


} // namespace
