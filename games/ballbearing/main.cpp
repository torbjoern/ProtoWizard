#include <proto/protographics.h>
#include <proto/color_utils.h>
#include <proto/fileio/text_file.h>
#include <proto/math/random.h>

#include <glm/gtc/quaternion.hpp>
#include "../../depends/obj_loader/objLoader.h"

#include "btBulletDynamicsCommon.h"

#include <iostream>
#include <string>
#include <vector>


namespace 
{
	btDiscreteDynamicsWorld* dynamicsWorld;
	std::vector<glm::vec3> constraint_lines;
	std::vector<btGeneric6DofConstraint*> constraints;
	protowizard::ProtoGraphicsPtr proto;
}

struct rigid_body_t
{
	glm::vec3 color;
	btRigidBody* bodyPtr;
	btCollisionShape* collisionShapePtr;
	std::string tri_mesh_path;
	// too polymorph or not

public:
	rigid_body_t(btRigidBody* rigidBody, btCollisionShape* collShape) :
				bodyPtr(rigidBody), collisionShapePtr(collShape)
	{
		color = glm::vec3(0.40f, 0.40f, 0.40f);
	}

	rigid_body_t(btRigidBody* rigidBody, btCollisionShape* collShape, const std::string &path) :
				bodyPtr(rigidBody), collisionShapePtr(collShape), tri_mesh_path(path)
	{
		color = glm::vec3(0.2f, 0.2f, 0.5f);
	}

	~rigid_body_t()
	{
		dynamicsWorld->removeRigidBody( bodyPtr );
		delete bodyPtr->getMotionState();
		delete bodyPtr;
		delete collisionShapePtr;
	}
};

std::vector<rigid_body_t*> phy_objs;
typedef std::vector<rigid_body_t*>::iterator rigid_list_iterator;
std::vector<rigid_list_iterator> phy_objs_to_be_deleted;

glm::vec3 toGLM(const btVector3 &v)
{
	return glm::vec3( v.x(), v.y(), v.z() );
}

btVector3 toBtVector( const glm::vec3 & v)
{
	return btVector3( v.x, v.y, v.z );
}

btRigidBody* makeRigid(const btTransform& trans, btCollisionShape* shape, float mass )
{
	btDefaultMotionState* fallMotionState = new btDefaultMotionState( trans );
	
    btVector3 fallInertia(0,0,0);
    shape->calculateLocalInertia(mass,fallInertia);
	
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,shape,fallInertia);
    btRigidBody* pBody = new btRigidBody(fallRigidBodyCI);
	//pBody->setDeactivationTime( btScalar(100.0f) );
    dynamicsWorld->addRigidBody(pBody);
	phy_objs.push_back( new rigid_body_t(pBody,shape) );
	return pBody;
}

btRigidBody* makeRigid(btCollisionShape* shape, glm::vec3 pos, float mass )
{
	btTransform trans(btQuaternion(0,0,0,1),toBtVector(pos) );
	return makeRigid( trans, shape, mass );
	btDefaultMotionState* fallMotionState = new btDefaultMotionState();
}

btVector3 obj2bt( obj_vector *v0 )
{
	return btVector3( (float)v0->e[0], (float)v0->e[1], (float)v0->e[2] );
}

btTriangleMesh* loadTriangleMesh(const std::string &fileName)
{
	btTriangleMesh *mTriMesh = new btTriangleMesh();

	//btTriangleIndexVertexArray( numTris, indexBasePtr, 0, numVerts, vertBasePtr, 0 );
	//proto->readMesh("assets/models/googley_chicken.obj");
	//mTriMesh->addTriangle(v0,v1,v2);


	objLoader *objData = new objLoader();

	char* cstrFileName = new char[fileName.length()+1];
	strcpy_s( cstrFileName, fileName.length()+1, fileName.c_str() );
	if ( !objData->load( cstrFileName ) ) 
	{
		// could not load mesh. 
		// TODO use place holder mesh. some red 3d text "PLACEHOLDER - MESH NOT FOUND"
		throw char ("Could prolly not find .obj file");
	}
	
	//std::vector< glm::vec3 > obj_vertex_coords;
	printf("loading obj model %s\n", cstrFileName);
	printf("Number of faces: %i\n", objData->faceCount);
	printf("Number of vertices: %i\n", objData->vertexCount);
	printf("\n");

	for(int i=0; i<objData->faceCount; i++)
	{
		if ( objData->faceList[i]->vertex_count <= 2 ) 
		{
				continue; // Hack. bad data
		}
	
		obj_face *facePtr = objData->faceList[i];

		obj_vector *v0 = (objData->vertexList[ facePtr->vertex_index[0] ]);
		obj_vector *v1 = (objData->vertexList[ facePtr->vertex_index[1] ]);
		obj_vector *v2 = (objData->vertexList[ facePtr->vertex_index[2] ]);
		mTriMesh->addTriangle( obj2bt(v0), obj2bt(v1), obj2bt(v2) );

		// Convert quads to tris
		// Load vert 3, and create new tri in order 0,2,3
		if ( objData->faceList[i]->vertex_count == 4 )
		{
			obj_vector *v3 = (objData->vertexList[ facePtr->vertex_index[3] ]);
			mTriMesh->addTriangle( obj2bt(v0), obj2bt(v2), obj2bt(v3) );
		}
	}

	delete(objData);
	return mTriMesh;
}

void createLevel(const glm::vec3 &pos, const std::string &objFilePath)
{
	btTriangleMesh* trimesh = loadTriangleMesh(objFilePath);
	btCollisionShape* triangle_shape = new btBvhTriangleMeshShape(trimesh,true);

	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
	// mass, motionState, collisionShape, localInertia
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,triangle_shape,btVector3(0,0,0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(groundRigidBody);
	
	phy_objs.push_back( new rigid_body_t(groundRigidBody,triangle_shape,objFilePath) );
}

btRigidBody* createBall(const glm::vec3 &pos, float mass = 1.f, float sphere_radius = 1.f)
{
	btCollisionShape* shape = new btSphereShape(sphere_radius);
	return makeRigid( shape, pos, mass );
}


btRigidBody* createCube( const btTransform& trans, float mass = 1.f, glm::vec3 halfExtents = glm::vec3(1.f) )
{
	btCollisionShape* shape = new btBoxShape( toBtVector(halfExtents) );
	return makeRigid( trans, shape, mass );
}

btRigidBody* createCube(const glm::vec3 &pos, float mass = 1.f, glm::vec3 halfExtents = glm::vec3(1.f) )
{
	btCollisionShape* shape = new btBoxShape( toBtVector(halfExtents) );
	return makeRigid( shape, pos, mass );
}

btRigidBody* createCapsule(const glm::vec3 &pos, float mass)
{
	btCollisionShape* shape = new btCapsuleShape(0.25f, 2.0f);
	return makeRigid( shape, pos, mass );
}

btRigidBody* createCylinder(const glm::vec3 &pos, float mass = 1.f, glm::vec3 halfExtents = glm::vec3(1.f) )
{
	btCollisionShape* shape = new btCylinderShape( toBtVector(halfExtents) );
	return makeRigid( shape, pos, mass );
}

void ray_test()
{
	protomath::Ray ray = proto->getMousePickRay();

	glm::vec3 start = -ray.origin;
	float rayLength = 100.0f;
	glm::vec3 end = start + ray.dir * rayLength;
	// Start and End are vectors
	btCollisionWorld::ClosestRayResultCallback RayCallback( toBtVector(start), toBtVector(end) );
 
	// Perform raycast
	dynamicsWorld->rayTest( toBtVector(start), toBtVector(end), RayCallback);
    
	

	if(RayCallback.hasHit()) {
		glm::vec3 end = toGLM(RayCallback.m_hitPointWorld);
		glm::vec3 normal = toGLM(RayCallback.m_hitNormalWorld);
		// Do some clever stuff here
		proto->setColor(0.4f, 0.4f, 0.1f);
		proto->drawCone(end, end+2.f*normal, 0.1f);

		proto->setBlend(true);
		proto->setAlpha( 0.75f );
		proto->drawSphere( end, 0.5f );
		proto->setBlend(false);

		if ( proto->keyhit('Q') )
		{
			createCube( end+normal, 1.f );
		}
		if ( proto->keyhit('W') )
		{
			createBall(end+normal, 1.f, 0.75f );
		}
		if ( proto->keyhit('F') )
		{
			btRigidBody* rigid = createBall(end+normal, 1.f, 0.75f );
			rigid->applyCentralImpulse( toBtVector( 10.f * proto->getCamera()->getLookDirection()) );
			phy_objs[ phy_objs.size()-1 ]->color = protowizard::hsv2rgb( protowizard::random(0.f, 360.f), 0.75f, 0.75f );
		}
		if ( proto->keyhit('A') )
		{
			createCapsule(end+normal, 1.f );
		}
		if ( proto->keyhit('S') )
		{
			createCylinder(end+normal, 1.f );
		}
		if ( proto->keystatus('E') )
		{
			for(size_t i=0; i<phy_objs.size(); i++){
				btRigidBody* bodyPtr = phy_objs[i]->bodyPtr;
				if ( RayCallback.m_collisionObject == bodyPtr ){
					bodyPtr->activate();
					//bodyPtr->applyCentralImpulse( toBtVector(3.f*normal) );
					btTransform trans = bodyPtr->getWorldTransform();
					btVector3 rel_pos = toBtVector(end) - trans.getOrigin();
					bodyPtr->applyImpulse( toBtVector(0.5f * normal), rel_pos );
				}
			}
			
		}
		if ( proto->keyhit('R') )
		{
			for(size_t i=0; i<phy_objs.size(); i++){
				btRigidBody* bodyPtr = phy_objs[i]->bodyPtr;
				if ( RayCallback.m_collisionObject == bodyPtr ){
					phy_objs[i]->color = protowizard::hsv2rgb( protowizard::random(0.f, 360.f), 0.75f, 0.75f );
				}
			}
		}

		if ( proto->keyhit('Z') )
		{
			createCube(end+normal, 1.f, glm::vec3(4.f, 1.f, 1.f) );
		}
		if ( proto->keyhit('X') )
		{
			createCube(end+normal, 1.f, glm::vec3(1.f, 1.f, 4.f) );
		}

		if ( proto->mouseDownRight() )
		{
			for(size_t i=0; i<phy_objs.size(); i++){
				btRigidBody* bodyPtr = phy_objs[i]->bodyPtr;
				if ( RayCallback.m_collisionObject == bodyPtr ){
					bodyPtr->activate();
					btTransform trans = bodyPtr->getWorldTransform();
					btVector3 rel_pos = toBtVector(end) - trans.getOrigin();
					bodyPtr->applyImpulse( toBtVector(normal), rel_pos );
				}
			}
		}

		if ( proto->keyhit(0) ) // mouseleft
		{
			printf("%.1f, %.1f, %.1f\n", end.x, end.y, end.z);
			//std::cout << end << std::endl;
		}

		if ( proto->keyhit(256+41) )
		{
			rigid_list_iterator it = phy_objs.begin();
			for(; it!=phy_objs.end(); ++it){
				if ( RayCallback.m_collisionObject == (*it)->bodyPtr ){
					phy_objs_to_be_deleted.push_back( it );
				}
			}
		}

		for(size_t i=0; i<phy_objs_to_be_deleted.size(); i++)
		{
			rigid_list_iterator it = phy_objs_to_be_deleted[i];
			delete (*it);
			phy_objs.erase( it );
		}
		
		phy_objs_to_be_deleted.clear();
	}
}

void draw_cylinder(const glm::vec3& pos, const glm::mat4& orientation, btCollisionShape*& shape )
{
	proto->setOrientation( glm::mat4(1.f) );

	const btCylinderShape* cylinder = static_cast<const btCylinderShape*>(shape);
	float radius = cylinder->getRadius();

	int upAxis = cylinder->getUpAxis();
	float halfHeight = cylinder->getHalfExtentsWithMargin()[upAxis];
	glm::vec3 p1 = glm::vec3( orientation * glm::vec4(0.f, halfHeight, 0.f, 0.f) );
	proto->drawCone( pos-p1 , pos+p1, radius );
}

void create_slider(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& extents = glm::vec3(1.0f, 0.125f, 2.5f) )
{ // 6DOF connected to the world, with motor
	glm::vec3 diff = p2 - p1;
	glm::vec3 axis = glm::normalize(diff);

	btVector3 sliderWorldPos( toBtVector(p1) );
	btVector3 sliderAxis( toBtVector(axis) );
	//btScalar angle=0.f;
	//btQuaternion ori_quat = btQuaternion(sliderAxis ,angle);
	//btMatrix3x3 sliderOrientation( ori_quat );
	
	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(sliderWorldPos);
	//trans.setRotation( btQuaternion( btVector3(0,0,1), 1.57f/2.f ) );
	
	// frameB.setRotation( ori_quat );
	btRigidBody* pBody = createCube(trans, 1.f, extents );
	pBody->setActivationState(DISABLE_DEACTIVATION);

	btTransform frameB;
	frameB.setIdentity();
	btGeneric6DofConstraint* pGen6Dof = new btGeneric6DofConstraint( *pBody, frameB, true );
	dynamicsWorld->addConstraint(pGen6Dof);
	pGen6Dof->setDbgDrawSize(btScalar(5.f));

	pGen6Dof->setAngularLowerLimit(btVector3(0,0,0));
	pGen6Dof->setAngularUpperLimit(btVector3(0,0,0));

	pGen6Dof->setLinearLowerLimit( btVector3(0,0,0) );
	pGen6Dof->setLinearUpperLimit( toBtVector(diff) );

	constraint_lines.push_back( p1 );
	constraint_lines.push_back( p2 );

	if ( axis.z == -1.f ) {
	pGen6Dof->getTranslationalLimitMotor()->m_enableMotor[2] = true;
	pGen6Dof->getTranslationalLimitMotor()->m_targetVelocity[2] = -1.5f;
	pGen6Dof->getTranslationalLimitMotor()->m_maxMotorForce[2] = 0.1f;
	}

	if ( fabs(axis.y) >= 1.f ) {
	pGen6Dof->getTranslationalLimitMotor()->m_enableMotor[1] = true;
	pGen6Dof->getTranslationalLimitMotor()->m_targetVelocity[1] = -2.5f;
	pGen6Dof->getTranslationalLimitMotor()->m_maxMotorForce[1] = 1.5f;
	}
	
	constraints.push_back( pGen6Dof );
	//dynamicsWorld->getConstraint(
}

void toggle_constraints()
{
	for(size_t i=0; i<constraints.size(); i++){
		btGeneric6DofConstraint* g6dof = constraints[i];
		btScalar pos = g6dof->getRelativePivotPosition(1);
		btScalar minp = g6dof->getTranslationalLimitMotor()->m_lowerLimit.y();
		btScalar maxp = g6dof->getTranslationalLimitMotor()->m_upperLimit.y();
		float dir = g6dof->getTranslationalLimitMotor()->m_targetVelocity.y();
		//if ( pos >= minp && dir < 0.f ){
		//	g6dof->getTranslationalLimitMotor()->m_targetVelocity[1] *= -1.f;
		//} else if (pos >= maxp && dir > 0.f ){
		//	g6dof->getTranslationalLimitMotor()->m_targetVelocity[1] *= -1.f;
		//}

		//if ( pos <= minp && dir >= 0.f || pos >= maxp && dir < 0.f ){
			g6dof->getTranslationalLimitMotor()->m_targetVelocity[1] *= -1.f;
		//} 

	}
}

void loadConstraints()
{
	TextFile text_file("steps.txt");

	for(int i=0; i<text_file.numLines(); i++)
	{
		text_file.setParseLine(i);
		float xcoord = text_file.getFloat();
		float ycoord = text_file.getFloat();
		float zcoord = text_file.getFloat();

		float xp = ycoord;
		float zp = xcoord;
		float yp = zcoord;

		// rotate (x,z) 90* ... should be enough to do (x,z) -> (-z,x) ?
		//float ca = cos( 1.57f );
		//float sa = sin( 1.57f );
		//float tx = xp; float ty = zp;
		//xp = ca*tx + sa*ty;
		//zp = sa*tx - ca*ty;

		glm::vec3 extents(2.8f, 0.125f, 1.45f);
		if ( i < 8 ) extents = glm::vec3(1.0f, 0.145f, 2.8f);
		create_slider( glm::vec3(xp,yp+2.f,zp), glm::vec3(xp,yp+3.6f,zp), extents );
	}
}



int main(int argc, const char* argv[])
{
	proto = ProtoGraphics::create();

	if (!proto->init(640,480,argv) ) {
		throw char("proto failed to init. probably shaders not found or GL drivers");
		return 1;
	}

	proto->setFrameRate( 60 );
	//proto->debugNormals(true);
	
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,-10,0));


	// Plane
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),1);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(groundRigidBody);
	
	const std::string level1_path = proto->getResourceDir() + "/models/level1.obj";
	createLevel(glm::vec3(0.f, 0.f, 0.f), level1_path );

	proto->setFrameRate( 60 );
	char title_buf[256];

	//loadConstraints();


	do
	{
		double time = proto->klock();

		proto->cls(0.f, 0.f, 0.f);

		float mspf = proto->getMSPF();
		sprintf_s( title_buf, 256, "%.2f mspf, %3.0f", 1000.f * mspf, 1.f/mspf);
		proto->setTitle( title_buf );

		float normalized_mx = proto->getMouseX() / (float)proto->getWindowWidth();
		float normalized_my = proto->getMouseY() / (float)proto->getWindowHeight();
		glm::vec2 normalized_mouse(normalized_mx, normalized_my);

		//float zoom = 15.f;// + proto->getMouseWheel();
		//proto->setCamera( glm::vec3(0.f, zoom, -zoom), glm::vec3(0.f, 1.0f, 0.f), glm::vec3(0.f, 1.f, 0.f) );
		//proto->enableFPSCamera();
        		
		dynamicsWorld->stepSimulation(1/60.f,10);

		static double time_since_last_toggle = 0.0;
		if ( time_since_last_toggle > 2.f ) {
			time_since_last_toggle = 0;
			toggle_constraints();
		}
		time_since_last_toggle += proto->getMSPF();

		// Draw Ground plane
		btTransform trans;
		groundRigidBody->getMotionState()->getWorldTransform(trans);
		proto->setColor(0.10f, 0.60f, 0.10f);
		proto->drawPlane( toGLM(trans.getOrigin()), glm::vec3(0.f, 1.f, 0.f), 50.f );
		
		// Draw all other physics objects
		for(size_t i=0; i<phy_objs.size(); i++)
		{
			btRigidBody *bodyPtr = phy_objs[i]->bodyPtr;
			btCollisionShape *shape = phy_objs[i]->collisionShapePtr;

			bodyPtr->getMotionState()->getWorldTransform(trans);
			btQuaternion ori = trans.getRotation();
			glm::mat4 mtx = glm::mat4_cast( glm::fquat(ori.w(), ori.x(), ori.y(), ori.z() ) );
			proto->setOrientation( mtx );

			glm::vec3 pos = toGLM(trans.getOrigin());

			proto->setColor( phy_objs[i]->color );

			int shapeType = shape->getShapeType();
			if ( shapeType == SPHERE_SHAPE_PROXYTYPE )
			{
				const btSphereShape* sphereShape = static_cast<const btSphereShape*>(shape);
				float radius = sphereShape->getMargin();//radius doesn't include the margin, so draw with margin
				proto->drawSphere( pos, radius );
			} 
			else if ( shapeType == BOX_SHAPE_PROXYTYPE )
			{
				const btBoxShape* boxShape = static_cast<const btBoxShape*>(shape);
				btVector3 halfExtent = boxShape->getHalfExtentsWithMargin();
				proto->setScale( 2.f * halfExtent.x(), 2.f * halfExtent.y(), 2.f * halfExtent.z() );
				proto->drawCube( pos );
				proto->setScale( 1.f );
			}
			else if ( shapeType == CAPSULE_SHAPE_PROXYTYPE )
			{
				proto->setOrientation( glm::mat4(1.f) );

				const btCapsuleShape* capsule = static_cast<const btCapsuleShape*>(shape);
				float radius = capsule->getRadius();

				float halfHeight = capsule->getHalfHeight();
				glm::vec3 p1 = glm::vec3( mtx * glm::vec4(0.f, halfHeight, 0.f, 0.f) );

				proto->drawCone( pos-p1 , pos+p1, radius );
				proto->drawSphere( pos - p1, radius );
				proto->drawSphere( pos + p1, radius );
			}
			else if ( shapeType == CYLINDER_SHAPE_PROXYTYPE )
			{
				draw_cylinder(pos, mtx, shape);
			}
			else if ( shapeType == TRIANGLE_MESH_SHAPE_PROXYTYPE )
			{
				proto->setBlend(true);
				proto->setAlpha(0.65f);
				proto->drawMesh( pos, phy_objs[i]->tri_mesh_path );
				proto->setBlend(false);
			}

			proto->setOrientation( glm::mat4(1.f) );
		}

		

		double time_begin = proto->klock();
		ray_test();
		double time_perf = proto->klock() - time_begin;

		proto->setColor(0.75f, 0.75f, 0.75f);
		for(size_t i=0; i<constraint_lines.size(); i+=2)
		{
			glm::vec3 p1 = constraint_lines[i+0];
			glm::vec3 p2 = constraint_lines[i+1];
			proto->drawCone( p1, p2, 0.1f );
		}

		glm::vec3 origin(0.f);
		proto->setColor(0.5f, 0.1f, 0.1f); proto->drawCone( origin, origin + proto->getCamera()->getStrafeDirection(), .1f );
		proto->setColor(0.1f, 0.5f, 0.1f); proto->drawCone( origin, origin + proto->getCamera()->getUpDirection(), .1f );
		proto->setColor(0.1f, 0.1f, 0.5f); proto->drawCone( origin, origin + proto->getCamera()->getLookDirection(), .1f );
		
		{
		using protowizard;
		proto->getCamera()->update( proto->keystatus(KEY::LEFT), proto->keystatus(KEY::RIGHT), proto->keystatus(KEY::UP), proto->keystatus(KEY::DOWN), (float)proto->getMouseX(), (float)proto->getMouseY(), proto->mouseDownLeft(), proto->getMSPF() );
		}

		proto->frame();
		
	} while( proto->isWindowOpen() );

    // Clean up behind ourselves like good little programmers
    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
	return 0;

}