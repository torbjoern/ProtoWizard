#pragma once

#include <glm/glm.hpp>

namespace protowizard{
struct Ray
{
	glm::vec3 origin;
	glm::vec3 dir;

	Ray(){
	}

	Ray(const glm::vec3& _origin, const glm::vec3& _dir ) : origin(_origin), dir(_dir)
	{
	}

	// if t is [0,1] then point is within ray
	// for example, t=0.5 means intersection is on middle of ray
	bool intersectBox( Ray ray, glm::vec3 box_pos, glm::vec3 box_dim, glm::vec3 &normal, float &t )
	{
		float EPSILON = 1e-5f;

		bool gotplane_origin[] = {true, true, true};
		bool gotplane_dim[] = {true, true, true};

		glm::vec3 dir = ray.dir;
		glm::vec3 ray_origin = ray.origin;

		for(int i=0; i<3; i++)
		{
			if ( fabs(dir[i]) <= EPSILON )
			{
				gotplane_origin[i] = false;
				gotplane_dim[i] = false;
			}else{

				if( dir[i] > 0 ) 
				{
					gotplane_dim[i] = false;
				}

				if ( dir[i] < 0 ) 
				{
					gotplane_origin[i] = false;
				}
			}

			if ( gotplane_dim[i]  )
			{   
				t = ( (box_pos[i]+box_dim[i]) - ray_origin[i]) / dir[i];
				glm::vec3 pt = ray_origin + dir * t;
				bool got1 = pt[ (i+1)%3 ] >= box_pos[ (i+1)%3 ] && pt[ (i+1)%3 ] <= (box_pos[ (i+1)%3 ]+box_dim[ (i+1)%3]) ;
				bool got2 = pt[ (i+2)%3 ] >= box_pos[ (i+2)%3 ] && pt[ (i+2)%3 ] <= (box_pos[ (i+2)%3 ]+box_dim[ (i+2)%3]) ;
				if ( got1 && got2 )
				{
					normal = glm::vec3( i==0, i==1, i==2 );
					return true;
				}
			}
			else if ( gotplane_origin[i] )
			{
				t = (box_pos[i] - ray_origin[i]) / dir[i];
				glm::vec3 pt = ray_origin + dir * t;
				bool got1 = pt[ (i+1)%3 ] >= box_pos[ (i+1)%3 ] && pt[ (i+1)%3 ] <= (box_pos[ (i+1)%3 ]+box_dim[ (i+1)%3]) ;
				bool got2 = pt[ (i+2)%3 ] >= box_pos[ (i+2)%3 ] && pt[ (i+2)%3 ] <= (box_pos[ (i+2)%3 ]+box_dim[ (i+2)%3]) ;
				if ( got1 && got2 )
				{
					normal = glm::vec3( (i==0)*-1, (i==1)*-1, (i==2)*-1 );
					return true;
				}
			}        

		} // end foreach axis (x,y,z)

		return false; 
	}
};

} // end namespace

