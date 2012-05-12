#include "global.h"

typedef float scalar;
const scalar infinity = FLT_MAX;
const glm::vec2 gravity( 0.f, 300.f );

typedef const glm::vec2& vecRef;

struct axis{
  glm::vec2 n;
  scalar d;
  axis() {}
  axis(vecRef n, scalar d) : n(n), d(d) {}
  static axis negate(const axis& a) { return axis(-a.n,a.d); };
};
typedef std::vector<axis> axisList;
typedef const axis& axisRef;

namespace vec
{
	glm::vec2 perp(const glm::vec2& v) {
		return glm::vec2( -v.y, v.x ); // Right normal
		//return glm::vec2( v.y, -v.x ); // Left normal
	}
	glm::vec2 polar(scalar a) {
		return glm::vec2( cos(a), sin(a) );
	}
	glm::vec2 rotate(const glm::vec2& a, const glm::vec2& b) {
		return glm::vec2( a.x*b.x - a.y*b.y, 
			              a.y*b.x + a.x*b.y );
	}
};

typedef std::vector<glm::vec2> vertexList;
struct poly
{
	vertexList verts;
	std::vector<axis> axes;

	poly() {}
	poly(vertexList& verts) {
		this->verts = verts;
		calcAxes();
	}
	
	void calcAxes() {
		for (size_t i=0; i<verts.size(); i++){
			const glm::vec2& v1 = verts[i];
			const glm::vec2& v2 = verts[(i+1)%verts.size()];
			glm::vec2 n = vec::perp( glm::normalize(v2 - v1) );
			axes.push_back( axis(n, glm::dot(n,v1)) );
		}
	}

	void draw( vecRef pos ) {
		// vertices
		for (size_t i=0; i<verts.size()+1; i++ ){
			const glm::vec2 &v = verts[i % verts.size()];
			if( i==0 )
				proto->moveTo( pos.x+v.x, pos.y+v.y );
			else
				proto->lineTo( pos.x+v.x, pos.y+v.y );
		}

		// axes
		for (size_t i=0; i<verts.size(); i++ ){
			const glm::vec2 &v1 = verts[i];
			const glm::vec2 &v2 = verts[(i+1) % verts.size()];
			glm::vec2 cent = pos + 0.5f * (v1+v2);

			axisRef ax = axes[i];
			proto->moveTo( cent.x, cent.y );
			proto->lineTo( cent.x + ax.n.x*10.f, cent.y+ax.n.y*10.f );
		}
	}
};
typedef const poly& polyRef;

struct body
{
	poly shape;
	scalar mass;
	scalar inertia;
	glm::vec2 pos;
	glm::vec2 vel;
	scalar ang;
	scalar rot;
	glm::vec2 snap;
	scalar asnap;

	body() {
	}

	body (float dummyArea, const poly& shape, const glm::vec2& pos) {
		this->shape = shape;
		mass = dummyArea * scalar(500);
		inertia = dummyArea * 500*500;

		this->pos = pos; 
		vel = glm::vec2(0.f);
		ang = scalar(0.0);
		rot = scalar(0.0);

		snap= glm::vec2(0.f);
		asnap = scalar(0.0);
	}

	void applySnap(const glm::vec2& j, const glm::vec2& r) {
		snap += scalar(1.0)/mass * j;
		asnap += scalar(1.0)/inertia * glm::dot( j, vec::perp(r) );

	}

	void applyJ(const glm::vec2& j, const glm::vec2& r){
		vel += scalar(1.0)/mass * j;
		rot += scalar(1.0)/inertia * glm::dot( j, vec::perp(r) );
	}

	// TODO makes no sense to use lambdas and transform here
	void updateShape() {
		auto dir = vec::polar( ang );
		auto &verts = shape.verts;
		auto toGlobalCoords = [&](const glm::vec2 &vert) { return vert+pos+ vec::rotate(dir,vert); };
		std::transform( verts.begin(), verts.end(), verts.begin(), toGlobalCoords ); 
		
		auto updateAxis = [&](axisRef a) -> axis { 
			glm::vec2 n = vec::rotate(a.n, dir);
			return axis(n, glm::dot(n,pos) + a.d); 
		};
		std::transform( shape.axes.begin(), shape.axes.end(), shape.axes.begin(), updateAxis ); 
	}

	void updateBody( scalar dt ) {
		pos = pos + dt * vel + snap;
		ang = ang + dt * rot + asnap;
		
		snap = glm::vec2(0.f);
		asnap = scalar(0.0);

		if ( mass != infinity ) {
			vel = vel + dt * gravity;
		}
	}

	void draw() {
		shape.draw( pos );
	}

};
typedef const body& bodyRef;

bool sepAxis( polyRef p1, polyRef p2, axis& collAxis ) {
	axisList axes;
	for (auto it=p1.axes.begin(); it!=p1.axes.end(); ++it) {
		axisRef ax = (*it);
		
		// Find smallest projected point (deepest isect)
		scalar smallest = infinity;
		for (auto it2=p2.verts.begin(); it2!=p2.verts.end(); ++it2 ){
			vecRef p2vert = (*it2);
			scalar dpr = glm::dot( ax.n,p2vert);
			if ( dpr < smallest ) {
				smallest = dpr;
			}
		}		
		axes.push_back( axis(ax.n, smallest - ax.d) );
	}
	
	// Find axis with smallest seperating distance, probably best dir to push apart
	collAxis.d = scalar(0.0);
	for (auto it=axes.begin(); it!=axes.end(); ++it) {
		axisRef a = (*it);
		if ( a.d > collAxis.d ) {
			collAxis = a;
		}
	}
	return collAxis.d <= 0.0f; // if distance is negative or zero, there was a coll or touch
}

// axis.d is the projection of the side onto the normal
// it acts as a zero point on the axis, this can tell us if a vertex v is over or under the line of the side with dot(a.n,v) - a.d
bool containsv( polyRef p, vecRef v ) {
	for( auto it = p.axes.begin(); it != p.axes.end(); ++it ) {
		axisRef a =  (*it);
		if(glm::dot( a.n, v) > a.d) {
			return false; 
		}
	}
	return true;
}

vertexList findvs( polyRef p1, polyRef p2, axisRef a ) {
	vertexList found;
	for ( auto it=p1.verts.begin(); it!=p1.verts.end(); ++it ) {
		if ( containsv(p2,*it) ){
			found.push_back( *it );
		}
	}

	for ( auto it=p2.verts.begin(); it!=p2.verts.end(); ++it ) {
		if ( containsv(p1,*it) ){
			found.push_back( *it );
			break;
		}
	}
	return found;
}

// Do polygon vs polygon collision test
// @returns a list of intersections, empty list if no collision
// TODO return ref to vertexList?
vertexList poly_poly( polyRef p1, polyRef p2 ) {	
	axis axis1;
	axis axis2;

	if ( sepAxis(p1, p2,axis1) ) {
		if ( sepAxis(p2, p1,axis2) ) {
			if ( axis1.d > axis2.d ) {
				return findvs(p1,p2,axis1);
			}else{
				return findvs(p1,p2, axis::negate(axis2) );
			}
		}
	}
	return vertexList(0);
}

scalar sq( scalar s ) {
	return s*s;
};

// <summary>
// Contact is the object used for handling each intersection point. Its task is to
// calculate how to push apart the two associated bodies
// </summary>
struct contact
{
	body& a; // ref's so we can mutate vel and pos
	body& b;
	/*const*/ scalar dist;
	int id;
	glm::vec2 p; // position
	glm::vec2 n; // normal
	glm::vec2 j; // impulse
	glm::vec2 jacc; // impulse
	/*const*/ glm::vec2 r1; // from a to p
	/*const*/ glm::vec2 r2; // from b to p
	typedef std::function<void (body& b, vecRef j, vecRef r)> MutateBodyFun;


	contact( body& a, body& b, scalar dist, vecRef n, vecRef p, int id ) :
		a(a), b(b), dist(dist), n(n), p(p), r1(p - a.pos), r2(p - b.pos)
	{
		//const glm::vec2 r1 = p - a.pos;
		//const glm::vec2 r2 = p - b.pos;
		jacc = glm::vec2(0.f);
		perform(); // perform once on create
	}

	glm::vec2 relative( vecRef bv, const scalar br, vecRef av, const scalar ar, vecRef r1, vecRef r2 ) const
	{
		return bv + br * vec::perp(r2 - av - ar * vec::perp(r1) );
	}

	// Actually pushing done here:
	void applyImpulse( MutateBodyFun f, vecRef j) {
		f(a,-j,r1); // a direction
		f(b,j,r2); // opposite dir
	};

	void perform()
	{
		const glm::vec2 relVel = relative( b.vel, b.rot, a.vel, a.rot, r1, r2 );
		const glm::vec2 relRot = relative( b.snap, b.asnap, a.snap, a.asnap, r1, r2 );

		auto kin = [](bodyRef a, bodyRef b, vecRef r1, vecRef r2, vecRef n) {
			return 1.0f / (
				1.0f/a.mass + 1.0f/b.mass +
				1.0f / a.inertia * sq( glm::dot(r1,vec::perp(n)) ) +
				1.0f / b.inertia * sq( glm::dot(r2,vec::perp(n)) )
				);
		};

		const auto massNormal = kin(a,b,r1,r2,n);
		const auto massTangent = kin(a,b,r1,r2, vec::perp(n) );
		// snap target
		const auto snapt = 0.2 * -std::min<scalar>( 0.0, dist+0.2 );

		const scalar sn = massNormal * (snapt - glm::dot(relRot,n) );
		scalar jn = massNormal * -glm::dot(relVel,n);
		scalar jt = massTangent * -glm::dot(relVel, vec::perp(n) );

		// Normal and tangential accumulated impulse
		// TODO
		// deltas? impulse accumulated in normal & tangent dir
		const scalar jnacc = jn;
		const scalar jtacc = jt;
		// impulse normal & tangent
		//jn = std::max( jn, 0.f );
		//jt = std::max( jt, jnacc*0.6f );


/// <summary>
/// Apply velocity and position corrections
/// </summary>
MutateBodyFun applySnap = []( body& b, vecRef j, vecRef r ) {
	b.snap += scalar(1.0)/b.mass * j;
	b.asnap += scalar(1.0)/b.inertia * glm::dot(j, vec::perp(r) );
};

/// <summary>
/// Apply impulse j, on body b. r is the displacement vector
// of the point at which the impulse is applied
// when j and r are perp it causes torque(?) verify this.
/// </summary>
MutateBodyFun applyJ = []( body& b, vecRef j, vecRef r ) {
	b.vel += scalar(1.0)/b.mass * j;
	b.rot += scalar(1.0)/b.inertia * glm::dot(j, vec::perp(r) );
};


		if ( sn > scalar(0.) ) applyImpulse( applySnap, sn*n );

		this->j = glm::vec2( jnacc, jtacc );
		//applyImpulse( applyJ, vec::rotate(n, glm::vec2(jn,jt) ) );

		// TODO
		//applyImpulse( applyJ, vec::rotate(n, j) );
		jacc = j;
	}
};

struct Collision
{
	vertexList pts;
	glm::vec2 n;
	scalar dist;
};

struct space
{
	scalar dt;
	std::vector<body> list;
	typedef std::shared_ptr<contact> contactPtr;
	std::vector<contactPtr> contacts;

	void generateContacts()
	{
		std::vector<contactPtr> cts;
		const size_t count = list.size();
		
		for (int i=0; i<count-1; i++){
			body& a = list[i];

			for (int j=i+1; j<count; j++){
				body& b = list[j];

				vertexList colPts = poly_poly( a.shape, b.shape );

				for ( auto it=colPts.begin(); it!=colPts.end(); ++it ) {
					
					vecRef point_p = *it;

					//Collision col;
					int id = -1; // todo
					glm::vec2 ab = a.pos - b.pos;
					scalar dist = glm::length(ab); // maybe distance between axii instead...
					glm::vec2 n = glm::normalize(ab);

					auto ct = contactPtr( new contact(a,b,dist, n, point_p, id) );
					for ( auto it2 = contacts.begin(); it2!=contacts.end(); ++it2 ) {
						const contactPtr &cont = (*it2);
						if ( cont->j == ct->j ) {
							cts.push_back( ct );
						}
					}
				}

			} // end for each j
		} // end for each i
		contacts = cts;
	} // end generate

	void update()
	{
		for(auto it=list.begin(); it!=list.end(); ++it ){
			it->updateBody(dt);
		}

		generateContacts();

		for(int i=0; i<10; i++){
			for( auto it=contacts.begin(); it!=contacts.end(); ++it) {
				(*it)->perform();
			}
		} // end sequential iteration
	} // end update

	void draw() {
		
		proto->setColor(1.f, 1.f, 1.f);
		for ( auto it=list.begin(); it!=list.end(); ++it ) {
			(*it).draw();
		}

		proto->setColor(0.f, 1.f, 0.f);
		for ( auto it=contacts.begin(); it!=contacts.end(); ++it ) {
			glm::vec2 p = (*it)->p;
			proto->drawCircle( p.x, p.y, 3.f );
		}

		if ( contacts.size() > 0 ) {
			printf("num cts: %d \n", contacts.size() );
		}
	}

};

poly makeBox(vecRef pos, scalar w, scalar h)
{
	vertexList verts;
	w *= 0.5f; h *= 0.5f;
	verts.push_back( pos+glm::vec2(-w,-h) );
	verts.push_back( pos+glm::vec2(-w,+h) );
	verts.push_back( pos+glm::vec2(+w,+h) );
	verts.push_back( pos+glm::vec2(+w,-h) );
	return poly( verts );
}

poly makeBox(scalar w, scalar h)
{
	vertexList verts;
	w *= 0.5f; h *= 0.5f;
	verts.push_back( glm::vec2(-w,-h) );
	verts.push_back( glm::vec2(-w,+h) );
	verts.push_back( glm::vec2(+w,+h) );
	verts.push_back( glm::vec2(+w,-h) );
	return poly( verts );
}
