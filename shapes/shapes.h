#ifndef _SHAPES_H
#define _SHAPES_H

#include "line.h"
#include "circle.h"
#include "sphere.h"
#include "cylinder.h"
#include "cube.h"
#include "plane.h"

#include "../vertex_types.h"
#include "../common.h"

class GeometryLibrary
{

public:
	bool init()
	{
		// TODO.. remove this, make it so line is a static class that checks if it
		// has a VBO on draw...
		if ( !line.init() )
		{
			printf("failed to init Line VBO");
			return false;
		}

		if ( !circle.init() )
		{
			printf("failed to init Circle VBO");
			return false;
		}

		if ( !sphere.init() )
		{
			printf("failed to init Sphere VBO");
			return false;
		}

		if ( !cylinder.init() )
		{
			printf("failed to init cylinder VBO");
			return false;
		}

		if ( !cube.init() )
		{
			printf("failed to init cube VBO");
			return false;
		}

		plane.init();
		GetError();

		return true;
	}

	void shutdown()
	{
		circle.shutdown();
		line.shutdown();
		sphere.shutdown();
		cube.shutdown();
		cylinder.shutdown();
		plane.shutdown();
	}

	Line line;
	Circle circle;
	Cylinder cylinder;
	Sphere sphere;
	Cube cube;
	Plane plane;
};

namespace blending
{
	enum BLEND_MODES
	{
		SOLID_BLEND = 1,
		ALPHA_BLEND,
		ADDITIVE_BLEND
	};
}

struct BaseState
{
	int blend_mode;
	glm::vec4 color;	
};

struct CircleState : public BaseState
{
	float x, y, radius;
};

struct BaseState3D : public BaseState
{
	float distance_from_camera( glm::vec3 const& camera_pos )
	{
		return glm::distance( glm::vec3(transform[3]), camera_pos );
	}

	bool is_transparent()
	{
		return blend_mode != blending::SOLID_BLEND;
	}

	virtual void pre_draw(Shader const& shader)
	{
		transform = glm::scale( transform, glm::vec3(radius) );

		int worldLoc = shader.GetVariable("worldMatrix");
		glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr( transform) );

		int isSphere = shader.GetVariable("isSphere");
		shader.SetInt(isSphere, 0);

		int locEmissive = shader.GetVariable("emissiveColor");
		shader.SetVec3(locEmissive, emissiveColor);
		

		//glm::mat4 model_view = viewMatrix * transform;
		//vsml->loadMatrix(VSML::MODELVIEW, glm::value_ptr( model_view ) );
		//vsml->matrixToUniform(VSML::MODELVIEW);
	}


	virtual void draw( GeometryLibrary* geo_lib ) = 0;

	BaseState3D()
	{
		transform = glm::mat4( 1.0f );
	}

	glm::mat4 transform;
	float radius;
	glm::vec3 emissiveColor;
};


struct SphereState : public BaseState3D
{
	virtual void pre_draw(Shader const& shader)
	{
		transform = glm::scale( transform, glm::vec3(radius) );

		int worldLoc = shader.GetVariable("worldMatrix");
		glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr( transform) );

		int locEmissive = shader.GetVariable("emissiveColor");
		shader.SetVec3(locEmissive, emissiveColor);

		int isSphere = shader.GetVariable("isSphere");
		shader.SetInt(isSphere, 1);
	}

	virtual void draw( GeometryLibrary* geo_lib )
	{
		geo_lib->sphere.draw();
	}
};

struct CylinderState : public BaseState3D
{
	virtual void pre_draw(Shader const& shader)
	{
		transform = glm::mat4(1.0f);

		int worldLoc = shader.GetVariable("worldMatrix");

		glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr( transform) );

		int isSphere = shader.GetVariable("isSphere");
		shader.SetInt(isSphere, 0);

		//vsml->loadMatrix(VSML::MODELVIEW, glm::value_ptr( viewMatrix ) );
		//vsml->matrixToUniform(VSML::MODELVIEW);
	}

	virtual void draw( GeometryLibrary* geo_lib )
	{
		geo_lib->cylinder.draw(p1,radius1,p2,radius2);
	}

	float distance_from_camera( glm::vec3 const& camera_pos )
	{
		return std::min<float>( glm::distance( p1, camera_pos ), glm::distance( p2, camera_pos ) );
	}

	glm::vec3 p1;
	glm::vec3 p2;
	float radius1;
	float radius2;
};

struct CubeState : public BaseState3D
{
	void draw( GeometryLibrary* geo_lib )
	{
		geo_lib->cube.draw();
	}
};

struct PlaneState : public BaseState3D
{
	virtual void draw( GeometryLibrary* geo_lib )
	{
		glm::vec3 pos = glm::vec3(transform[3]);
		glDisable(GL_CULL_FACE);
		geo_lib->plane.createGeometry( pos, normal, 1.0 );
		geo_lib->plane.draw();
		//geo_lib->plane.createGeometry( pos + normal*0.01f, -normal, 1.0);
		//geo_lib->plane.draw();

		glEnable(GL_CULL_FACE);
	}

	glm::vec3 normal;
};



#endif
