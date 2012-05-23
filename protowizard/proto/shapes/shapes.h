#pragma once;

#include "line.h"
#include "circle.h"
#include "../geo_sphere.h"
#include "../geo_cylinder.h"
#include "../geo_cube.h"
#include "../geo_plane.h"
#include "../shapes/mesh.h"

#include "../mesh_manager.h"
#include "mesh.h"

#include "../vertex_types.h"
#include "../opengl_stuff.h"

#include <map>
#include <memory>
#include <vector>

namespace protowizard{
struct Shapes_t
{
	LineGeometry line;
	CircleGeometry circle;


	bool init()
	{
		// TODO.. remove this, make it so line is a static class that checks if it
		// has a VBO on draw...
		if ( !line.init() )
		{
			printf("failed to init Line VBO");
			return false;
		}

		bool inited = true;
		inited &= circle.init();
		inited &= SphereGeometry::init();
		inited &= CylinderGeometry::init();
		inited &= CubeGeometry::init();
		inited &= PlaneGeometry::init();

		if ( !inited ) {
			printf("failed to init some shape");
		}
		return inited;
	}
	void de_init()
	{
		circle.shutdown();
		line.shutdown();
		SphereGeometry::shutdown();
		CubeGeometry::shutdown();
		CylinderGeometry::shutdown();
		PlaneGeometry::shutdown();
	}
} Shapes;

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
	BaseState() { }
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

class BaseState3D : public BaseState
{
  public:
	glm::mat4 transform;
	unsigned int tex_handle;
	void (*drawPtr) (void);
	MeshPtr mesh;

	BaseState3D() {} 
	
	BaseState3D ( const glm::vec4& color
	              , int blend_mode
				  , const glm::mat4& transform
				  , unsigned int tex_handle
				  , std::vector<BaseState3D>& opaque
				  , std::vector<BaseState3D>& translucent
				  , void (*drawPtr) (void)
				  ) 
				  : BaseState(color, blend_mode)
				  , transform(transform)
				  , tex_handle(tex_handle)
				  , drawPtr (drawPtr)
				  , mesh(nullptr)
	{
		if( blend_mode == blending::SOLID_BLEND )
		{
			opaque.push_back( *this );
		}else{
			translucent.push_back( *this );
		}
	}

	BaseState3D ( const glm::vec4& color
	              , int blend_mode
				  , const glm::mat4& transform
				  , unsigned int tex_handle
				  , std::vector<BaseState3D>& opaque
				  , std::vector<BaseState3D>& translucent
				  , const MeshPtr mesh
				  ) 
				  : BaseState(color, blend_mode)
				  , transform(transform)
				  , tex_handle(tex_handle)
				  , drawPtr (nullptr)
				  , mesh(mesh)
	{
		if( blend_mode == blending::SOLID_BLEND )
		{
			opaque.push_back( *this );
		}else{
			translucent.push_back( *this );
		}
	}

	float distance_from_camera( glm::vec3 const& camera_pos )
	{
		return glm::distance( glm::vec3(transform[3]), camera_pos );
	}

	bool is_transparent()
	{
		return blend_mode != blending::SOLID_BLEND;
	}

	void setBlendMode() const
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

	void pre_draw(const Shader &shader, bool useTexture) const
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

	void draw() const
	{
		if ( drawPtr ) {
			(*drawPtr)();
		} else {
			mesh->draw();
		}
	}
};

} 

