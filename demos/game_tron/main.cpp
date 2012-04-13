#define GLM_SWIZZLE 
#include <glm/glm.hpp>

#include "protographics.h"
#include "math/math_common.h"
#include "math/math_line.h"
#include "color_utils.h"

#include <functional>
#include <cassert>
#include <iostream>
#include <vector>
#include <map>

typedef ProtoMath::line_t<glm::vec3> line3D;
typedef ProtoMath::line_t<glm::vec2> line2D;

void draw_wall_segment( ProtoGraphics &proto, const glm::vec3 start, const glm::vec3 end, float thickness = 0.25f ) {
	float siz_x = fabs(start.x - end.x);
	float siz_z = fabs(start.z - end.z);
	float kEpsilon = 1e-6f;
	if ( siz_x <= kEpsilon ){
		siz_x = thickness;
	}
	if ( siz_z <= kEpsilon ){
		siz_z = thickness;
	}
	proto.setScale( 0.5f * siz_x, 1.0f, 0.5f * siz_z );

	glm::vec3 center = 0.5f * ( start + end );
	proto.drawCube( center );
}

struct particle_t {
	glm::vec3 pos, vel;
	double flight_time;

	particle_t( const glm::vec3 &pos_, const glm::vec3 &vel_ ) : pos(pos_), vel(vel_), flight_time(0.0) {
	}
};


struct particle_emitter_t{
	double seconds_to_live;
	double emission_rate_per_second;
	double spawn_time_out;
	glm::vec3 pos;
	bool dead;

	std::vector< particle_t > particles;

	particle_emitter_t( double seconds_to_live_, const glm::vec3& pos_ ) : seconds_to_live(seconds_to_live_), 
																			emission_rate_per_second(10.0), 
																			spawn_time_out(0.0), 
																			pos(pos_), 
																			dead(false) 
	{
	}

	void draw( ProtoGraphics& proto ){

		float speed = 1.0f / 60.0f;
		const glm::vec3 gravity(0.f, -1.0f, 0.f);

		proto.setBlend( true );
		proto.setAlpha( 0.5f );
		proto.setColor( 1.f, 1.f, 0.f );
		for ( size_t i=0; i<particles.size(); i++ ){
			proto.drawSphere( particles[i].pos, 0.5f );
			float tSq = (float) (particles[i].flight_time*particles[i].flight_time);
			particles[i].pos += speed * particles[i].vel + speed*(0.5f * gravity * tSq); // y = y0 + 0.5 * g * t^2
		}
		proto.setBlend( false );
	}

	void update( ProtoGraphics& proto, double dt ){
		seconds_to_live -= dt;
		spawn_time_out -= dt;

		if ( seconds_to_live <= 0.0 ) {
			bool dead = true;
		} else {

			for(auto it=begin(particles); it!=end(particles); ++it){
				(*it).flight_time += dt;
			}

			if ( spawn_time_out < 0.0 ) {
				spawn_time_out = 1.0 / emission_rate_per_second;
				glm::vec3 vel = glm::vec3( proto.sfrand() * 0.5f, 5.0f * fabs(proto.sfrand()), proto.sfrand() * 0.5f  );
				particles.push_back( particle_t(pos, vel ) );
			}
		}
	}
};

struct particle_emitter_manager_t{
	static std::vector<particle_emitter_t> emitter_list;

	static void create_emitter( double seconds_to_live, const glm::vec3& pos ) {
		particle_emitter_t emitter( seconds_to_live, pos );
		emitter_list.push_back( emitter );
	}

	static void draw_all( ProtoGraphics& proto ) {
		
		for( auto it=begin(emitter_list); it!=end(emitter_list); ++it ){
			if ( (*it).seconds_to_live > 0.0 ) {
				(*it).draw(proto);
			}
		}
	}

	static void update_all( ProtoGraphics& proto, double dt ) {
		
		for( auto it=begin(emitter_list); it!=end(emitter_list); ++it ){
			if ( (*it).seconds_to_live > 0.0 ) {
				(*it).update(proto,dt);
			}
		}


		//auto itr = dead[i];
		//*itr = emitter_list.back();
		//emitter_list.pop_back();
	}

	static void clear() {
		emitter_list.clear();
	}
};

std::vector<particle_emitter_t> particle_emitter_manager_t::emitter_list; // keep c++ compiler happy

struct player_registration_t{
	bool alive;
};

typedef std::vector<player_registration_t > Player_Registery;

struct player_t{
	glm::vec3 pos;
	glm::vec3 player_front_point;
	glm::vec3 player_back_point;
	glm::vec2 dir;
	glm::vec2 velocity;
	float speed;
	glm::vec3 player_color;
	float heading;

	std::vector< glm::vec3 > tail_points;

	double time_until_change_dir;

	const double DIR_CHANGE_COOLDOWN_SECS;
	const float MINIMUM_SPEED;
	const float MAXIMUM_SPEED;

	Player_Registery &player_registery;
	int id;

	int steer_key_left;
	int steer_key_right;

	float RADIUS_SQUARED;
	float player_radius;
	float player_length;

	bool is_hugging_tail;

	player_t( Player_Registery &player_registery_, int id_, int steer_key_left_, int steer_key_right_ ) : 
															   DIR_CHANGE_COOLDOWN_SECS( 0.125 ), 
															   MINIMUM_SPEED( 1.0f / 16.0f ),
															   MAXIMUM_SPEED( 1.0f / 8.0f ),
															   player_registery( player_registery_ ),
															   id(id_),
															   steer_key_left( steer_key_left_ ), steer_key_right( steer_key_right_ ),
															   is_hugging_tail( false )
	{
		player_registration_t me;
		me.alive = true;
		player_registery.push_back( me );

		player_radius = 0.5f;
		RADIUS_SQUARED = player_radius * player_radius;
		player_length = 2.0f * player_radius;

		speed = MINIMUM_SPEED;

		
	}

	void init(){
		player_registery[id].alive = true;

		heading = 0.f + (M_PI / 2.0f) * id;
		
		dir = glm::vec2( sin(heading), cos(heading) );
		player_front_point = pos + player_length * glm::vec3(dir.x, 0.f, dir.y);
		player_back_point = pos - player_length * glm::vec3(dir.x, 0.f, dir.y);
		velocity = speed * dir;
		pos = glm::vec3( 0.f, 1.f, 0.f ) + 4.0f * player_radius * glm::vec3( dir.x, 0.f, dir.y );

		float normalized_color = heading / (M_PI);
		player_color = protowizard::hsv2rgb( 360.f * normalized_color , 1.f, 1.f );

		time_until_change_dir = 0.f;

		tail_points.clear();
		create_tail_point();
	}

	void draw_tail( ProtoGraphics &proto ){
		proto.setBlend( true );
		proto.setAlpha( 0.75f );
		proto.setColor( player_color );

		unsigned int num_points = (unsigned int)tail_points.size();

		if ( num_points > 1 ){
			draw_wall_segment( proto, tail_points[0], tail_points[1] );
			
			for( unsigned int i = 1; i<num_points; i++ ){
				draw_wall_segment( proto, tail_points[i], tail_points[i-1] );
			}
		}

		// Draw from last created tail-point to player
		if ( num_points > 0 ){
			draw_wall_segment( proto, pos, tail_points[num_points-1] );
		}
			
		proto.setScale( 1.f, 1.f, 1.f );
		proto.setOrientation( glm::mat4(1.0f) );
		proto.setBlend( false );
	}

	void draw( ProtoGraphics &proto ){
		


		if ( speed > MINIMUM_SPEED ) {
			proto.setColor( lerp(proto.random(0.0f, 1.0f), glm::vec3(1.f,1.f,0.f), player_color) ) ;
		} else {
			proto.setColor( player_color );
		}

		float ang = glm::degrees( heading );
		
		proto.setScale( 0.25f, 0.5f, player_length );
		proto.setOrientation( glm::rotate( glm::mat4(1.0f), ang, glm::vec3(0.f, 1.f, 0.f) ) );
		
		proto.drawCube( pos );
		proto.setScale( 1.f, 1.f, 1.f );
		proto.setOrientation( glm::mat4(1.0f) );

		// Debug viz
		float feeler_range = 2.5f * player_radius;
		glm::vec2 feeler_vector = glm::normalize( velocity );
				
		glm::vec2 perp1( -feeler_vector.y, feeler_vector.x );
		glm::vec2 perp2( feeler_vector.y, -feeler_vector.x );
		
		proto.drawCone( player_front_point, player_front_point + glm::vec3(perp1.x, 0.f, perp1.y ), 0.25f );
		proto.drawCone( player_front_point, player_front_point + glm::vec3(perp2.x, 0.f, perp2.y ), 0.25f );

		// debug viz

		draw_tail( proto );

		//char buf[256];
		//sprintf(buf, "heading: %.2f", heading);
		//proto.drawText3D( pos, buf );
	}

	void update( ProtoGraphics &proto, double dt ){
		//float dx = (float)( proto.keystatus( GLFW_KEY_RIGHT ) - proto.keystatus( GLFW_KEY_LEFT ) );
		float steer_dir = (float) ( proto.keystatus(steer_key_left) - proto.keystatus( steer_key_right ) );
		//float steer_dir = (float) ( proto.keyhit(steer_key_left) - proto.keyhit( steer_key_right ) );

		if ( time_until_change_dir <= 0.0 ){
			if (  fabs(steer_dir) > 0.f ){
				heading += steer_dir * (M_PI / 2.f);
				time_until_change_dir = DIR_CHANGE_COOLDOWN_SECS;
				create_tail_point();
			}
		}

		if ( is_hugging_tail ) {
			float acceleration = 0.25f * (float)dt;
			speed += acceleration;
			if ( speed > MAXIMUM_SPEED ) {
				speed = MAXIMUM_SPEED;
			}
			is_hugging_tail = false;
		}

		if ( speed > MINIMUM_SPEED ) {
			float retardation = 0.1f * (float)dt;
			speed -= retardation;
		}

		dir = glm::vec2( sin(heading), cos(heading) );
		player_front_point = pos + player_length * glm::vec3(dir.x, 0.f, dir.y);
		player_back_point = pos - player_length * glm::vec3(dir.x, 0.f, dir.y);
		velocity = speed * dir;
		pos += glm::vec3(velocity.x, 0.f, velocity.y);

		time_until_change_dir -= dt;

		test_tail_collision( *this );
	}

	void create_tail_point(){
		tail_points.push_back( pos );
	}

	void crash_wall()  {
		// particle effects go boom
		particle_emitter_manager_t::create_emitter( 5.0, pos );
		player_registery[ this->id ].alive = false;
	}

	void hugging_tail() {
		is_hugging_tail = true;
	}

	void test_tail_collision( player_t &other ) {
		size_t num_points = tail_points.size();

		for (size_t i=0; i<num_points; i++){
			glm::vec3 v0 = tail_points[i];
			
			glm::vec3 v1;

			// when we're on the last stored point, create a line from last stored tail_point to player pos
			if ( i==num_points-1 ) {
				v1 = pos;
				if ( &other == this ){
					v1 = other.pos;
					continue;
				}
			}else{
				v1 = tail_points[i+1];
			}

			//glm::vec3 v0v1 = v1 - v0;
			//if ( glm::length(v0v1) < 4.0f*player_radius ) continue;
			
			

			line2D velocity_vector( other.pos.xz(), other.pos.xz() + other.velocity );
			line2D tail_vector( v0.xz(), v1.xz() );
			bool isect = ProtoMath::find_intersection( velocity_vector, tail_vector );
	
			if ( isect ) {
				if ( &other != this ){
					std::cout << "player " << id << " scores ! " << std::endl;
				} else {
					std::cout << "player " << id << " collided with self... " << std::endl;
				}
				other.crash_wall();
				return;
			} else {
				float feeler_range = 2.5f * player_radius;
				glm::vec2 feeler_vector = glm::normalize( other.velocity );
				
				glm::vec2 perp1( -feeler_vector.y, feeler_vector.x );
				glm::vec2 perp2( feeler_vector.y, -feeler_vector.x );
				line2D feeler_side1( other.player_front_point.xz(), other.player_front_point.xz() + perp1 );
				line2D feeler_side2( other.player_front_point.xz(), other.player_front_point.xz() + perp2 );
				bool isect_feeler1 = ProtoMath::find_intersection( feeler_side1, tail_vector );
				bool isect_feeler2 = ProtoMath::find_intersection( feeler_side2, tail_vector );
				
				if (isect_feeler1 || isect_feeler2 ) other.hugging_tail();
			}

		} // end for each tail point
	}
};


struct board_t {

	board_t () : board_size(30.f),
				 half_size(0.5f * board_size)
	{
	}

	void draw(ProtoGraphics& proto){
		proto.setColor( 0.f, 0.f, 1.f );

		for (size_t i=0; i<wall_segments.size(); i++){
			draw_wall_segment( proto, wall_segments[i].v0, wall_segments[i].v1, 2.f );
		}

		proto.drawPlane( glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f), half_size );
	}

	void init() {

		glm::vec3 p0( -half_size, 0.f, -half_size );
		glm::vec3 p1( +half_size, 0.f, -half_size );
		glm::vec3 p2( +half_size, 0.f, +half_size );
		glm::vec3 p3( -half_size, 0.f, +half_size );

		wall_segments.push_back( line3D(p0, p1) );
		wall_segments.push_back( line3D(p1, p2) );
		wall_segments.push_back( line3D(p2, p3) );
		wall_segments.push_back( line3D(p3, p0) );
	}

	void test_collision( player_t &player ) {

		for (size_t i=0; i<wall_segments.size(); i++){
			line2D velocity_vector( player.pos.xz(), player.pos.xz() + player.velocity );
			line2D wall_segment2D( wall_segments[i].v0.xz(), wall_segments[i].v1.xz() );
			
			bool isect = ProtoMath::find_intersection( velocity_vector, wall_segment2D );

			if ( isect ) {
				player.crash_wall();
				return;
			}
		}
		
	}

private:
	std::vector< line3D > wall_segments;
	float board_size;
	float half_size;
};


int main(int argc, const char* argv[])
{
	ProtoGraphics proto;
	bool inited = proto.init(640,480,argv);
	if( !inited )
	{
		throw char("could not init ProtoGraphics");
		return 1;
	}


	
	int num_frames = 0;

	//proto.setCamera( glm::vec3(20.f, -20.f, -20.f), 45.0f, 45.0f );
	proto.setCamera( glm::vec3(0.f, -22.f, 0.f), 0.0f, 90.0f );
	

	Player_Registery player_registery;
	std::vector< player_t > player_list;

	player_t player0( player_registery, 0, 'Z', 'X' );
	player_t player1( player_registery, 1, ',', '.' );

	player_list.push_back( player0 );
	player_list.push_back( player1 );

	for( size_t i=0; i<player_list.size(); i++ ) {
		player_list[i].init();
	}

	

	board_t board;
	board.init();

	double old_time = 0;

	struct {
		bool running;
	} game_state;

	game_state.running = true;

	proto.setFrameRate( 60 );
	
	while( proto.isWindowOpen() )
	{
		double time = proto.klock();
		double dt = time - old_time;
		old_time = time;

		// TODO ... a good lerping camera
		glm::vec3 ahead_of_player = player_list[0].pos + 10.f * glm::vec3( player_list[0].dir.x, 0.f, player_list[0].dir.y );
		glm::vec3 behind_of_player = player_list[0].pos - 10.f * glm::vec3( player_list[0].dir.x, 0.f, player_list[0].dir.y );
		proto.setCamera( glm::vec3(0.f, 10.f, 0.f) + behind_of_player, ahead_of_player, glm::vec3(0.f, 1.f, 0.f) );

		proto.cls(0,0,0);

		proto.setColor( 1.f, 1.f, 1.f );

		board.draw( proto );

		for( size_t i=0; i<player_list.size(); i++){
			player_list[i].draw( proto );
		}

		if ( game_state.running == true ) {


		int num_players_alive = 0;
		for( size_t i=0; i<player_list.size(); i++){
			player_t &player = player_list[i];

			if ( player_registery[player.id].alive ) {
				num_players_alive++;
				player.update( proto, dt );
				board.test_collision( player );
			}
		}

		for( auto it1=begin(player_list); it1!=end(player_list); ++it1 ){
			player_t &player1 = (*it1);
			//if ( player_registery[player1.id].alive == false ) continue;

			for( auto it2=begin(player_list); it2!=end(player_list); ++it2 ){
				if ( it2 == it1 ) continue;

				player_t &player2 = (*it2);
				if ( player_registery[player2.id].alive == false ) continue;
				player1.test_tail_collision( player2 );
			}	
		}

			if ( num_players_alive <= 1 ) {
				game_state.running = false;
				std::cout << "game ended, press R to reset" << std::endl;
			}
		}

		if ( game_state.running == false ) {

			if ( proto.keyhit('R') ) {
				for( size_t i=0; i<player_list.size(); i++ ) {
					player_list[i].init();

					particle_emitter_manager_t::clear();
				}

				game_state.running = true;
			}
		}



		particle_emitter_manager_t::draw_all( proto );
		particle_emitter_manager_t::update_all( proto, dt );


		//while( proto.klock() < num_frames/60.f ) {}

		num_frames++;

		proto.frame();
	}
	return 0;
}