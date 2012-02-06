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
	bool init();

	void shutdown()
	{
		circle.shutdown();
		line.shutdown();
		sphere.shutdown();
		cube.shutdown();
		cylinder.shutdown();
		plane.shutdown();
	}

	LineGeometry line;
	CircleGeometry circle;
	CylinderGeometry cylinder;
	SphereGeometry sphere;
	CubeGeometry cube;
	PlaneGeometry plane;
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
	virtual float distance_from_camera( glm::vec3 const& camera_pos )
	{
		return glm::distance( glm::vec3(transform[3]), camera_pos );
	}

	bool is_transparent()
	{
		return blend_mode != blending::SOLID_BLEND;
	}

	virtual void pre_draw(Shader const& shader)
	{
		if ( tex_handle != 0 )
		{
			// http://www.opengl.org/wiki/GLSL_Samplers#Binding_textures_to_samplers
			// TODO sort by material/texture to avoid unneccessary swappign and binds?
			int loc = shader.GetVariable("tex0");
			shader.SetInt( loc, 0 );
			glActiveTexture(GL_TEXTURE0 + 0); // Texture Unit to use
			glBindTexture(GL_TEXTURE_2D, tex_handle); // texture_handle to bind to currently active unit
		}

		int worldLoc = shader.GetVariable("worldMatrix");
		glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr( transform) );

		int isSphere = shader.GetVariable("isSphere");
		shader.SetInt(isSphere, 0);

		// would be most effective to send readily calculated MVP directly to GL
		//glm::mat4 model_view_projection = projectionMatrix * viewMatrix * modelMatrix;
	}


	virtual void draw( GeometryLibrary* geo_lib ) = 0;

	BaseState3D()
	{
		transform = glm::mat4( 1.0f );
		tex_handle = 0;
	}

	glm::mat4 transform;
	unsigned int tex_handle;
};

struct MeshState : public BaseState3D
{
	virtual void draw( GeometryLibrary* geo_lib );

	std::string mesh_path;
};


struct SphereState : public BaseState3D
{
	void *operator new(size_t size);
    void operator delete(void *memory);

	virtual void pre_draw(Shader const& shader);

	virtual void draw( GeometryLibrary* geo_lib );

private:
	static std::vector<SphereState*> pool;

};

struct CylinderState : public BaseState3D
{
	void *operator new(size_t size);
	void operator delete(void *memory);


	virtual void pre_draw(Shader const& shader);

	virtual void draw( GeometryLibrary* geo_lib );

	virtual float distance_from_camera( glm::vec3 const& camera_pos ) const;

	glm::vec3 p1;
	glm::vec3 p2;
	float radius;

private:
	static std::vector<CylinderState*> pool;
};

struct CubeState : public BaseState3D
{
	void *operator new(size_t size);

    void operator delete(void *memory);

	void draw( GeometryLibrary* geo_lib )
	{
		geo_lib->cube.draw();
	}

private:
	static std::vector<CubeState*> pool;
};

struct PlaneState : public BaseState3D
{
	void *operator new(size_t size);

	void operator delete(void *memory);

	virtual void draw( GeometryLibrary* geo_lib )
	{
		glm::vec3 pos = glm::vec3(transform[3]);
		glDisable(GL_CULL_FACE);
		geo_lib->plane.createGeometry( pos, normal, 1.0 );
		geo_lib->plane.draw();

		glEnable(GL_CULL_FACE);
	}

	glm::vec3 normal;

private:
	static std::vector<PlaneState*> pool;
};



#endif
