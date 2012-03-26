#include <protographics.h>
#include <glm/gtc/quaternion.hpp>

#include "color_utils.h"
#include "color_utils.h"
#include "../../depends/obj_loader/objLoader.h"

#include "btBulletDynamicsCommon.h"


#pragma comment(lib, "F:/code_lab/bullet-2.79/lib/BulletDynamics_debug.lib")
#pragma comment(lib, "F:/code_lab/bullet-2.79/lib/BulletCollision_debug.lib")
//#pragma comment(lib, "F:/code_lab/bullet-2.79/lib/BulletSoftBody_debug.lib")
//#pragma comment(lib, "F:/code_lab/bullet-2.79/lib/ConvexDecomposition_debug.lib")
//#pragma comment(lib, "F:/code_lab/bullet-2.79/lib/HACD_debug.lib")
#pragma comment(lib, "F:/code_lab/bullet-2.79/lib/LinearMath_debug.lib")
//#pragma comment(lib, "F:/code_lab/bullet-2.79/lib/OpenGLSupport_debug.lib")

namespace 
{
	btDiscreteDynamicsWorld* dynamicsWorld;
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

btRigidBody* makeRigid( btDynamicsWorld* dynamicsWorld, btCollisionShape* shape, glm::vec3 pos, float mass )
{
	btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),toBtVector(pos) ));
	
    btVector3 fallInertia(0,0,0);
    shape->calculateLocalInertia(mass,fallInertia);
	
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,shape,fallInertia);
    btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
    dynamicsWorld->addRigidBody(fallRigidBody);
	phy_objs.push_back( new rigid_body_t(fallRigidBody,shape) );
	return fallRigidBody;
}

btVector3 obj2bt( obj_vector *v0 )
{
	return btVector3( (float)v0->e[0], (float)v0->e[1], (float)v0->e[2] );
}

btTriangleMesh* loadTriangleMesh(const std::string &fileName)
{
	btTriangleMesh *mTriMesh = new btTriangleMesh();

	//btTriangleIndexVertexArray( numTris, indexBasePtr, 0, numVerts, vertBasePtr, 0 );
	//proto.readMesh("assets/models/googley_chicken.obj");
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

void createLevel(btDiscreteDynamicsWorld *dynamicsWorld, const glm::vec3 &pos, const std::string &objFilePath)
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

btRigidBody* createBall(btDiscreteDynamicsWorld *dynamicsWorld, const glm::vec3 &pos, float mass = 1.f, float sphere_radius = 1.f)
{
	btCollisionShape* shape = new btSphereShape(sphere_radius);
	return makeRigid( dynamicsWorld, shape, pos, mass );
}

btRigidBody* createCube(btDiscreteDynamicsWorld *dynamicsWorld, const glm::vec3 &pos, float mass = 1.f, glm::vec3 halfExtents = glm::vec3(1.f) )
{
	btCollisionShape* shape = new btBoxShape( toBtVector(halfExtents) );
	return makeRigid( dynamicsWorld, shape, pos, mass );
}

btRigidBody* createCapsule(btDiscreteDynamicsWorld *dynamicsWorld, const glm::vec3 &pos, float mass)
{
	btCollisionShape* shape = new btCapsuleShape(0.5f, 1.5f);
	return makeRigid( dynamicsWorld, shape, pos, mass );
}

btRigidBody* createCylinder(btDiscreteDynamicsWorld *dynamicsWorld, const glm::vec3 &pos, float mass = 1.f, glm::vec3 halfExtents = glm::vec3(1.f) )
{
	btCollisionShape* shape = new btCylinderShape( toBtVector(halfExtents) );
	return makeRigid( dynamicsWorld, shape, pos, mass );
}

void ray_test(btDiscreteDynamicsWorld *dynamicsWorld)
{
	ProtoGraphics* protoPtr = ProtoGraphics::getInstance();
	protomath::Ray ray = protoPtr->getMousePickRay();

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
		protoPtr->setColor(0.4f, 0.4f, 0.1f);
		protoPtr->drawCone(end, end+2.f*normal, 0.1f);

		protoPtr->setBlend(true);
		protoPtr->setAlpha( 0.75f );
		protoPtr->drawSphere( end, 0.5f );
		protoPtr->setBlend(false);

		if ( protoPtr->keyhit('Q') )
		{
			createCube(dynamicsWorld, end+normal, 1.f );
		}
		if ( protoPtr->keyhit('W') )
		{
			createBall(dynamicsWorld, end+normal );
		}
		if ( protoPtr->keyhit('A') )
		{
			createCapsule(dynamicsWorld, end+normal, 1.f );
		}
		if ( protoPtr->keyhit('S') )
		{
			createCylinder(dynamicsWorld, end+normal, 1.f );
		}
		if ( protoPtr->keyhit('E') )
		{
			for(size_t i=0; i<phy_objs.size(); i++){
				btRigidBody* bodyPtr = phy_objs[i]->bodyPtr;
				if ( RayCallback.m_collisionObject == bodyPtr ){
					bodyPtr->activate();
					bodyPtr->applyCentralImpulse( toBtVector(3.f*normal) );
					btTransform trans = bodyPtr->getWorldTransform();
					btVector3 rel_pos = toBtVector(end) - trans.getOrigin();
					bodyPtr->applyImpulse( toBtVector(normal), rel_pos );
				}
			}
			
		}
		if ( protoPtr->keyhit('R') )
		{
			for(size_t i=0; i<phy_objs.size(); i++){
				btRigidBody* bodyPtr = phy_objs[i]->bodyPtr;
				if ( RayCallback.m_collisionObject == bodyPtr ){
					phy_objs[i]->color = protowizard::hsv2rgb( protoPtr->random(0.f, 360.f), 0.75f, 0.75f );
				}
			}
		}

		if ( protoPtr->keyhit('Z') )
		{
			createCube(dynamicsWorld, end+normal, 1.f, glm::vec3(4.f, 1.f, 1.f) );
		}
		if ( protoPtr->keyhit('X') )
		{
			createCube(dynamicsWorld, end+normal, 1.f, glm::vec3(1.f, 1.f, 4.f) );
		}

		if ( protoPtr->keyhit(256+41) )
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
	ProtoGraphics* protoPtr = ProtoGraphics::getInstance();
	protoPtr->setOrientation( glm::mat4(1.f) );

	const btCylinderShape* cylinder = static_cast<const btCylinderShape*>(shape);
	float radius = cylinder->getRadius();

	int upAxis = cylinder->getUpAxis();
	float halfHeight = cylinder->getHalfExtentsWithMargin()[upAxis];
	glm::vec3 p1 = glm::vec3( orientation * glm::vec4(0.f, halfHeight, 0.f, 0.f) );
	protoPtr->drawCone( pos-p1 , pos+p1, radius );
}

int main()
{
	ProtoGraphics proto;

	if (!proto.init(640,480) ) {
		throw char("proto failed to init. probably shaders not found or GL drivers");
		return 1;
	}


	proto.setFrameRate( 60 );
	//proto.debugNormals(true);
	
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

	const std::string level1_path = "assets/models/level1.obj";
	createLevel(dynamicsWorld, glm::vec3(0.f, 0.f, 0.f), level1_path );

	proto.setFrameRate( 60 );

	char title_buf[256];
	do
	{
		double time = proto.klock();

		proto.cls(0.f, 0.f, 0.f);

		float mspf = proto.getMSPF();
		sprintf_s( title_buf, 256, "%.2f mspf, %3.0f", 1000.f * mspf, 1.f/mspf);
		proto.setTitle( title_buf );

		float normalized_mx = proto.getMouseX() / (float)proto.getWindowWidth();
		float normalized_my = proto.getMouseY() / (float)proto.getWindowHeight();
		glm::vec2 normalized_mouse(normalized_mx, normalized_my);

		//float zoom = 15.f;// + proto.getMouseWheel();
		//proto.setCamera( glm::vec3(0.f, zoom, -zoom), glm::vec3(0.f, 1.0f, 0.f), glm::vec3(0.f, 1.f, 0.f) );
		//proto.enableFPSCamera();
        
		
		dynamicsWorld->stepSimulation(1/60.f,10);
 
		// Draw Ground plane
		btTransform trans;
		groundRigidBody->getMotionState()->getWorldTransform(trans);
		proto.setColor(0.10f, 0.60f, 0.10f);
		proto.drawPlane( toGLM(trans.getOrigin()), glm::vec3(0.f, 1.f, 0.f), 50.f );
		
		// Draw all other physics objects
		for(size_t i=0; i<phy_objs.size(); i++)
		{
			btRigidBody *bodyPtr = phy_objs[i]->bodyPtr;
			btCollisionShape *shape = phy_objs[i]->collisionShapePtr;

			bodyPtr->getMotionState()->getWorldTransform(trans);
			btQuaternion ori = trans.getRotation();
			glm::mat4 mtx = glm::mat4_cast( glm::fquat(ori.w(), ori.x(), ori.y(), ori.z() ) );
			proto.setOrientation( mtx );

			glm::vec3 pos = toGLM(trans.getOrigin());

			proto.setColor( phy_objs[i]->color );

			int shapeType = shape->getShapeType();
			if ( shapeType == SPHERE_SHAPE_PROXYTYPE )
			{
				const btSphereShape* sphereShape = static_cast<const btSphereShape*>(shape);
				float radius = sphereShape->getMargin();//radius doesn't include the margin, so draw with margin
				proto.drawSphere( pos, radius );
			} 
			else if ( shapeType == BOX_SHAPE_PROXYTYPE )
			{
				const btBoxShape* boxShape = static_cast<const btBoxShape*>(shape);
				btVector3 halfExtent = boxShape->getHalfExtentsWithMargin();
				proto.setScale( 2.f * halfExtent.x(), 2.f * halfExtent.y(), 2.f * halfExtent.z() );
				proto.drawCube( pos );
				proto.setScale( 1.f );
			}
			else if ( shapeType == CAPSULE_SHAPE_PROXYTYPE )
			{
				proto.setOrientation( glm::mat4(1.f) );

				const btCapsuleShape* capsule = static_cast<const btCapsuleShape*>(shape);
				float radius = capsule->getRadius();

				float halfHeight = capsule->getHalfHeight();
				glm::vec3 p1 = glm::vec3( mtx * glm::vec4(0.f, halfHeight, 0.f, 0.f) );

				proto.drawCone( pos-p1 , pos+p1, radius );
				proto.drawSphere( pos - p1, radius );
				proto.drawSphere( pos + p1, radius );
			}
			else if ( shapeType == CYLINDER_SHAPE_PROXYTYPE )
			{
				draw_cylinder(pos, mtx, shape);
			}
			else if ( shapeType == TRIANGLE_MESH_SHAPE_PROXYTYPE )
			{
				proto.drawMesh( pos, phy_objs[i]->tri_mesh_path );
			}

			proto.setOrientation( glm::mat4(1.f) );
		}

		double time_begin = proto.klock();
		ray_test(dynamicsWorld);
		double time_perf = proto.klock() - time_begin;

		glm::vec3 origin(0.f);
		proto.setColor(0.5f, 0.1f, 0.1f); proto.drawCone( origin, origin + proto.getCamera()->getStrafeDirection(), .1f );
		proto.setColor(0.1f, 0.5f, 0.1f); proto.drawCone( origin, origin + proto.getCamera()->getUpDirection(), .1f );
		proto.setColor(0.1f, 0.1f, 0.5f); proto.drawCone( origin, origin + proto.getCamera()->getLookDirection(), .1f );
		
		
		proto.frame();
		
	} while( proto.isWindowOpen() );

    // Clean up behind ourselves like good little programmers
    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
	return 0;

}