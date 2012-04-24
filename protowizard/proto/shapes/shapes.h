#ifndef _SHAPES_H
#define _SHAPES_H

#include "line.h"
#include "circle.h"
#include "sphere.h"
#include "cylinder.h"
#include "cube.h"
#include "plane.h"

#include "../mesh_manager.h"
#include "mesh.h"

#include "../vertex_types.h"
#include "../common.h"

#include <map>
#include <memory>


namespace Shapes
{
	extern LineGeometry line;
	extern CircleGeometry circle;
	extern CylinderGeometry cylinder;
	extern SphereGeometry sphere;
	extern CubeGeometry cube;
	extern PlaneGeometry plane;

	bool init();
	void de_init();
}

namespace blending
{
	enum BLEND_MODES
	{
		SOLID_BLEND = 1, // no blending
		ALPHA_BLEND, // GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA
		ADDITIVE_BLEND, // GL_SRC_ALPHA,GL_ONE
	};
}

struct BaseState
{
	int blend_mode;
	glm::vec4 color;
	BaseState() {
	}
	BaseState(const glm::vec4& color, int blend_mode) : color(color), blend_mode(blend_mode)
	{
	}
	
};

struct CircleState : public BaseState
{
	float x, y, radius;
	CircleState(const glm::vec4& color, int blend_mode, float x, float y, float radius) : 
				BaseState(color, blend_mode), x(x), y(y), radius(radius) {}
};

struct BaseState3D : public BaseState
{
	glm::mat4 transform;
	unsigned int tex_handle;

	BaseState3D()
	{
		transform = glm::mat4( 1.0f );
		tex_handle = 0;
	}
	BaseState3D ( const glm::vec4& color, int blend_mode, const glm::mat4& transform, unsigned int tex_handle ) : 
				  BaseState(color, blend_mode), transform(transform), tex_handle(tex_handle)
	{
	}

	
	virtual float distance_from_camera( glm::vec3 const& camera_pos )
	{
		return glm::distance( glm::vec3(transform[3]), camera_pos );
	}

	bool is_transparent()
	{
		return blend_mode != blending::SOLID_BLEND;
	}

	void setBlendMode()
	{
		//http://www.opengl.org/sdk/docs/man/xhtml/glBlendFunc.xml
		switch ( blend_mode )
		{
		case blending::ADDITIVE_BLEND:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Don't care about alpha of what we write over
			break;
		case blending::ALPHA_BLEND:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // blending incoming and inversely what was there before
			break;
		}
	}

	virtual void pre_draw(Shader const& shader, bool useTexture)
	{
		if ( useTexture ) {
			if ( tex_handle != 0 )
			{
				shader.SetInt( shader.GetVariable("use_textures"), 1 );

				// http://www.opengl.org/wiki/GLSL_Samplers#Binding_textures_to_samplers
				// TODO sort by material/texture to avoid unneccessary swappign and binds?
				int loc = shader.GetVariable("tex0");
				shader.SetInt( loc, 0 );
				glActiveTexture(GL_TEXTURE0 + 0); // Texture Unit to use
				glBindTexture(GL_TEXTURE_2D, tex_handle); // texture_handle to bind to currently active unit
			} else {
				shader.SetInt( shader.GetVariable("use_textures"), 0 );
			}
		}

		setBlendMode();

		int worldLoc = shader.GetVariable("worldMatrix");
		glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr( transform) );

		// would be most effective to send readily calculated MVP directly to GL
		//glm::mat4 model_view_projection = projectionMatrix * viewMatrix * modelMatrix;
	}

	virtual void draw() = 0;
};

struct MeshState : public BaseState3D
{
	virtual void draw() {
		mesh->draw();
	}
	MeshPtr mesh;
};

struct SphereState : public BaseState3D
{
	virtual void draw() {
		Shapes::sphere.draw();
	}
};

struct CylinderState : public BaseState3D
{
	virtual void draw() {
		Shapes::cylinder.draw( hasCap );
	}
	bool hasCap;
};

struct CubeState : public BaseState3D
{
	void draw() {
		Shapes::cube.draw();
	}
};

struct PlaneState : public BaseState3D
{
	virtual void draw()
	{
		glm::vec3 pos = glm::vec3(transform[3]);
		glDisable(GL_CULL_FACE);
		// TODO
		Shapes::plane.createGeometry( pos, normal, 1.0 );
		Shapes::plane.draw();

		glEnable(GL_CULL_FACE);
	}

	glm::vec3 normal;
};



#endif
