#ifndef _CYLINDER_H
#define _CYLINDER_H

#include "../common.h"

class CylinderGeometry
{
	public:
	CylinderGeometry()
	{
		num_cylinder_vertices = num_topcap_vertices = num_bottomcap_vertices = 0;
	}
	~CylinderGeometry()
	{

	}

	void shutdown()
	{
		// http://www.opengl.org/sdk/docs/man/xhtml/glDeleteBuffers.xml
		glDeleteBuffers( 1, &cylinderBufferObject);	
		glDeleteVertexArrays(1, &cylinderVAO);

		glDeleteBuffers( 1, &topcapBufferObject);	
		glDeleteVertexArrays(1, &topcapVAO);

		glDeleteBuffers( 1, &bottomcapBufferObject);	
		glDeleteVertexArrays(1, &bottomcapVAO);
	}

	void wind_cap(std::vector< PNVertex > &vertices, float wind_dir, float radius, float yPos, int segments)
	{
		glm::vec3 normal = glm::normalize(glm::vec3(0.f, wind_dir, 0.f)) ;

		int j = 0;
		for(int i=0; i<segments; i++){

			int idx = 0;
			if( i > 0 && i % 2 == 0 )
			{
				idx = segments-j+1;
			}else{

				idx = j; 
				j++;
			}  

			float a = wind_dir * idx*( TWO_PI/(segments-1.0f) );

			float ca = cos(a);
			float sa = sin(a);
			float x = ca*radius;
			float z = sa*radius;      
 
			PNVertex v1 ( glm::vec3(x, yPos, z), normal );
			vertices.push_back( v1 );
		}

	}

	void cylinder(std::vector< PNVertex > &vertices, float radius, float length, int segments)
	{
		using glm::vec3;
		float angInc = TWO_PI / (segments-1.f);
		float a = 0.f;
		for(int i=0; i<segments; i++){
			float ca = cos(a);
			float sa = sin(a);
			float x = ca*radius;
			float z = sa*radius;

			vec3 p1 = vec3(x, 0.f, z);
			vec3 p2 = vec3(x, length, z);
			PNVertex v1 ( p1, glm::normalize(p1 - vec3(0.f, 0.f, 0.f)  ) );
			PNVertex v2 ( p2, glm::normalize(p2 - vec3(0.f, length, 0.f)  ) );
			vertices.push_back( v1 );
			vertices.push_back( v2 );

			a -= angInc;
		}
	}

	void fill_vao( std::vector<PNVertex> &verts, unsigned int vao, unsigned int vbo )	
	{
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(PNVertex) * verts.size(), &verts[0], GL_STATIC_DRAW);
		glBindVertexArray(0);
	}

	void create_geometry()
	{
		std::vector< PNVertex > cylinder_wall;
		std::vector< PNVertex > cylinder_top;
		std::vector< PNVertex > cylinder_bottom;

		float radius = 1.0f;
		float length = 1.0f;
		int segments = 16;

		cylinder(cylinder_wall, radius, length, segments );
		wind_cap(cylinder_top, 1.0f, radius, length, segments);
		wind_cap(cylinder_bottom, -1.0f, radius, 0.f, segments);

		num_cylinder_vertices = cylinder_wall.size();
		num_topcap_vertices = cylinder_top.size();
		num_bottomcap_vertices = cylinder_bottom.size();

		fill_vao( cylinder_wall, cylinderVAO, cylinderBufferObject );
		fill_vao( cylinder_top, topcapVAO, topcapBufferObject );
		fill_vao( cylinder_bottom, bottomcapVAO, bottomcapBufferObject );

		
	}
	
	void draw( bool draw_cap )
	{
		if ( !draw_cap ){
			glDisable(GL_CULL_FACE);
		}

		glBindVertexArray(cylinderVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, num_cylinder_vertices );
		glBindVertexArray(0);

		if ( !draw_cap ){
			glEnable(GL_CULL_FACE);
		}

		if ( draw_cap ){
			glBindVertexArray(topcapVAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, num_topcap_vertices );
			glBindVertexArray(0);

			glBindVertexArray(bottomcapVAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, num_bottomcap_vertices );
			glBindVertexArray(0);
		}
	}
	
	// TODO can this function fail?

	void init_vao( unsigned int &vao, unsigned int &vbo )
	{
		vbo = 0;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof( PNVertex ), 0);
#define BUFFER_OFFSET(p) ((char*)0 + (p))
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof( PNVertex ), BUFFER_OFFSET(12) ); 

		glBindVertexArray(0);
	}

	bool init()
	{
		init_vao( cylinderVAO, cylinderBufferObject );
		init_vao( topcapVAO, topcapBufferObject );
		init_vao( bottomcapVAO, bottomcapBufferObject );

		create_geometry();

		return true;
	}
	
	private:
		unsigned int cylinderBufferObject;
		unsigned int cylinderVAO;

		unsigned int topcapBufferObject;
		unsigned int topcapVAO;

		unsigned int bottomcapBufferObject;
		unsigned int bottomcapVAO;

		int num_cylinder_vertices;
		int num_topcap_vertices;
		int num_bottomcap_vertices;
		

};


#endif
