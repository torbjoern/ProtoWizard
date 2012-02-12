
#include "protographics.h"
#include "math/math_common.h"

#include <iostream>
#include <functional>
#include <cassert>




#define CX 8
#define CY 8
#define CZ 8

typedef char uint8_t;

struct chunk {
	uint8_t blk[CX][CY][CZ];
	bool changed;

	chunk() {
		memset(blk, 0, sizeof blk);
		changed = true;
	}

	~chunk() {
	}

	uint8_t get(int x, int y, int z) {
		return blk[x][y][z];
	}

	void set(int x, int y, int z, uint8_t type) {
		blk[x][y][z] = type;
		changed = true;
	}

	void update() {
		changed = false;
		// Fill in the VBO here
	}

	void render(ProtoGraphics &protoGraphics, glm::vec3 parent_pos) {
		if(changed)
			update();
		// Render the VBO here
		for (int x=0; x<CX; x++)
		{
			for (int y=0; y<CY; y++)
			{
				for (int z=0; z<CZ; z++)
				{
					if ( blk[x][y][z] )
					{
						protoGraphics.setScale( 0.5f );
						protoGraphics.drawCube( parent_pos + glm::vec3(x,y,z) );
						protoGraphics.setScale( 1.0f );
					}
					
				}
			}
		}
		

	}
};

#define SCX 4
#define SCY 4
#define SCZ 4

struct superchunk {
	chunk *c[SCX][SCY][SCZ];

	superchunk() {
		memset(c, 0, sizeof c);
	}

	~superchunk() {
		for(int x = 0; x < SCX; x++)
			for(int y = 0; y < SCX; y++)
				for(int z = 0; z < SCX; z++)
					delete c[x][y][z];
	}

	uint8_t get(int x, int y, int z) {
		int cx = x / CX;
		int cy = y / CY;
		int cz = z / CZ;

		x %= CX;
		y %= CY;
		z %= CZ;

		if(!c[cx][cy][cz])
			return 0;
		else
			return c[cx][cy][cz]->get(x, y, z);
	}

	void set(int x, int y, int z, uint8_t type) {
		int cx = x / CX;
		int cy = y / CY;
		int cz = z / CZ;

		x %= CX;
		y %= CY;
		z %= CZ;

		if(!c[cx][cy][cz])
			c[cx][cy][cz] = new chunk();

		c[cx][cy][cz]->set(x, y, z, type);
	}

	void render( ProtoGraphics &protoGraphics ) {
		for(int x = 0; x < SCX; x++)
			for(int y = 0; y < SCX; y++)
				for(int z = 0; z < SCX; z++)
					if(c[x][y][z])
						c[x][y][z]->render( protoGraphics, glm::vec3(x * CX, y * CY, z * CZ) );
	}
};

class Ray;
bool isect_ray_box( Ray ray, glm::vec3 box_pos, glm::vec3 box_dim, glm::vec3 &normal, float &t );

class Cube{

public:
	Cube(int _x, int _y, int _z, float half_size_){
		x = _x;
		y = _y;
		z = _z;
		half_size = half_size_;
	}

	void draw(ProtoGraphics &proto){
		proto.setScale( half_size );
		proto.drawCube( glm::vec3(x,y,z) );
		proto.setScale( 1.f );
	}


	float x;
	float y;
	float z;
	float half_size;
};

class Ray{
public:
	glm::vec3 origin;
	glm::vec3 dir;

	Ray(glm::vec3 o, glm::vec3 d) {
		origin = o;
		dir = d;
	}

	bool isectCube( Cube c ){
		float half_size = c.half_size;
		glm::vec3 pos ( c.x - half_size , c.y - half_size, c.z - half_size  );
		glm::vec3 dim ( half_size*2.f );
		glm::vec3 normal;//( 0,0,0 );
		float tmin;// = 0.0;
		bool hit = isect_ray_box( Ray(origin, dir), pos, dim, normal, tmin);
		return (hit && tmin > 0.0 );
	}

	void draw( ProtoGraphics &proto ){
		glm::vec3 end = origin + dir;
		proto.drawCone( origin, end, 0.1f );
	}
};


// if t is [0,1] then point is within ray
// for example, t=0.5 means intersection is on middle of ray
bool isect_ray_box( Ray ray, glm::vec3 box_pos, glm::vec3 box_dim, glm::vec3 &normal, float &t )
{
	float EPSILON = 0.00001f;

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


void scene(ProtoGraphics &proto)
{
	int mousx = proto.getMouseX();
	int mousy = proto.getMouseY();

	float ma = M_PI + mousx / (float)proto.getWindowWidth() * TWO_PI;
	
	proto.setColor(1,1,1);
	
	Ray charles( glm::vec3(-5,2.0,4.0), glm::vec3(15.0 * cos(ma), 0.0, 18.0 * sin(ma) ) );
	charles.draw(proto);

	ma += M_PI;
	Ray man( glm::vec3(15,4.0,4.0), glm::vec3(15.0 * cos(ma), 0.0, 18.0 * sin(ma) ) );
	man.draw(proto);


	int x = 0;
	int y = 0; 
	int z = 0;
	int side_by_side = 12;
	int tot = (int)pow( (float)side_by_side, 3.f );
	for(int i=0; i<tot; i++){
		float scale = 0.5f;
		Cube c( scale*x, scale*y, scale*z, scale);

		x++;
		if ( x >= side_by_side ){
			x = 0;
			y++;
		}

		if ( y >= side_by_side ){
			y = 0;
			z++;
		}




		bool hit = false;

		if ( charles.isectCube( c ) )
		{
			hit = true;
			proto.setColor(0,0,1);
		}

		if( man.isectCube(c) ){
			hit = true;
			proto.setColor(0,1,0);
		}

		if (hit)
		{
			proto.setAlpha(1.0f);
			proto.setBlend(false);
		} 
		else
		{
			proto.setColor(1,1,1);
			proto.setAlpha(0.25f);
			proto.setBlend(true);
		}
		
		proto.disableTexture();
		c.draw( proto );

	}
	proto.setBlend(false);

}

void increment( void ){
}

class Timer{
public:
	typedef std::tr1::function <void(void)> voidfunc;
	explicit Timer(voidfunc callback_, double interval_) : callback(callback_), interval(interval_) {
		start_time = ProtoGraphics::getInstance()->klock();

	}

	void onTick( ) {
		callback();
	}

	void update(){
		double time = ProtoGraphics::getInstance()->klock();
		if ( (time - start_time) > interval ) {
			start_time = time;
			onTick();
		}
	}

private:
	double interval;
	double start_time;
	std::tr1::function <void(void)> callback;
};

void printfunc(void){
	printf("Hello, from printfunc\n");
}

struct IncFunctor
{
	IncFunctor( double &var_ ) : var(var_) {
	}

	void operator() (){
		var++;
	}


	double &var;
};

void draw_line_cube( ProtoGraphics& proto, glm::vec3 pos , float scale )
{
		for(int i=0; i<2; i++){
		for(int j=0; j<2; j++){
			float u = -1.0f + i*2.0f;
			float v = -1.0f + j*2.0f;
			glm::vec3 p1 ( u, v, -1.f ); glm::vec3 p2 ( u, v, 1.0f );
			p1 *= scale * 0.5f;
			p2 *= scale * 0.5f;

			p1 += pos;
			p2 += pos;

			proto.drawCone( p1, p2 , 1.0 );

			proto.drawCone( p1.xzy(), p2.xzy() , 1.0 );

			proto.drawCone( p1.zxy(), p2.zxy() , 1.0 );
				
		}
	}
}

int main()
{
	using glm::vec3;

	ProtoGraphics proto;
	if (proto.init(640,480) == false )
	{
		std::cerr << "proto failed to init" << std::endl;
		return 1;
	}

	

	double seconds = 0.0;
	IncFunctor my_functor( seconds );

	Timer secTimer( my_functor , 1.0 / 4.0  );

	do
	{
		secTimer.update();

		proto.cls(0,0,0);

		float normalized_mx = proto.getMouseX() / (float) proto.getWindowWidth();
		float normalized_my = proto.getMouseY() / (float) proto.getWindowHeight();
		proto.setCamera( vec3(-0.f, 3.f, -10.f -normalized_my * 15.f), vec3(0.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f) );



	//draw_line_cube( proto, glm::vec3( world_scale*0.5f ), world_scale );

		
		//for (int i=0; i<20; i++)
		//{
		//	for (int j=0; j<20; j++)
		//	{
		//		float u = -0.5f + i/20.f;
		//		float v = -0.5f + j/20.f;
		//		float dist = glm::length( glm::vec2( u,v ) - glm::vec2(0.f) );
		//		proto.setScale( 1.f, 1.0, 0.1f + 1.0f / dist );
		//		proto.drawCube( vec3( u*40.f, v*40.f, 0.f +  0.1f + 1.0f / dist), 0.5f );
		//	}
		//}

		scene(proto);
		proto.setColor( 1, 1, 1);
		//universe.render(proto);

		proto.setColor( 1, 1, 0 );
		proto.moveTo( 0.f, 0.f );
		proto.lineTo( 100.f, 100.f );

		proto.setColor( 1, 0, 0);
		proto.drawCircle(50, 50, 30 );

		proto.setColor( 0, 0, 1 );
		proto.moveTo( 100.f, 0.f );
		proto.lineTo( 0.f, 100.f );



		proto.frame();
	}while( proto.isWindowOpen() );
	return 0;
}