
#include <proto/protographics.h>
#include <proto/shapes/mesh.h>
#include <proto/platform.h>
#include <proto/mesh_manager.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <string>

// blender camera behaviour
// middle mouse button rotates camera around lookAt point using arcball
// shift+middle mouse translates lookAt and lookFrom by same amount

//[x] arcball rotation
//[ ] translate like p3d.in
//[ ] diffuse texture
//[ ] use .mtl file
//[x] exe file doesn't have to be in same dir as .obj
//[ ] drag'n drop
//[ ] choose light type. pos vs dir. add multiple lights.
//[ ] shadowmap

// arcball code: http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
glm::vec3 getArcballVector( glm::vec2 point ) 
{
	glm::vec3 p( point.x, point.y, 0.f );
	p.y = -p.y;
	float pSq = p.x*p.x + p.y*p.y;
	if ( pSq <= 1.f ) {
		p.z = sqrt( 1.f - pSq );
	} else {
		p = glm::normalize(p);
	}
	return p;
}

void calcArc( glm::vec2 &cur, glm::vec2 &last, const glm::mat4 &cameraTransform, glm::mat4 &object2world )
{
	if( cur.x != last.x || cur.y != last.y ) {
		glm::vec3 va = getArcballVector( last );
		glm::vec3 vb = getArcballVector( cur );
		float angle = acos( std::min(1.0f, glm::dot(va,vb) ) );
		glm::vec3 axisInCameraCoord = glm::cross(va,vb);
		glm::mat3 camera2object = glm::inverse(glm::mat3( cameraTransform * object2world ));
		glm::vec3 axisInObjectCoord = camera2object * axisInCameraCoord;
		object2world = glm::rotate(object2world, glm::degrees(angle), axisInObjectCoord );
		last = cur;
	}
}

int main(int argc, char* argv[])
{
	std::string filename = "";

	if ( argc < 2 ) {
		
		filename = protowizard::OpenFileDlg("c:\\", "obj" );
		if ( filename == "" ) return 0;
	} else {
		puts("args:");
		for (int i=0; i<argc; i++){ puts(argv[i]); }
		puts("end args");
	
		filename = std::string( argv[1] );
		if ( filename.find( ".obj" ) == std::string::npos ) {
			puts("expected .obj extension");
			return 1;
		}
	}
	
	protowizard::ProtoGraphics proto;
	// would be nice if this failed automatically...
	// resource dir should find shaders regardless working directory, 
	// expect that shaders are near .exe somewhere. in ../assets/ or /assets for instance
	// this requires win32 code to find exe location
	if ( proto.init(1024,1024, "" /*protowizard::GetExePath()*/ ) == false ){
		protowizard::ShowMessageBox("failed to init proto", "failed to init");
		return 1;
	}
	
	proto.setColor(.7f, .7f, .7f);
	
	
	protowizard::MeshPtr mesh = nullptr;
	try{
		mesh = protowizard::MeshManager::getMesh( filename );
		if ( mesh == nullptr ) {
			throw "mesh is null";
		}
	} catch ( std::exception e ){
		puts( e.what() );
		protowizard::ShowMessageBox(e.what(), "load mesh");
		exit(1);
	}
	
	const glm::vec3 dims = mesh->getDimensions();
	printf("dimensions: %.2f %.2f %.2f\n", dims.x, dims.y,  dims.z);
	glm::vec3 meshScale = glm::vec3(1.0f / glm::length(dims));
	
	// Need to know mesh extents for zoom. Calc Mesh Bounding Box! This is also useful for culling!
	float cameraZoom = 1.f;
	
	
	bool twoSided = true;
	bool showNormals = false;
	
	glm::mat4 modelMatrix(1.f);
	glm::vec3 meshTranslate(0.f);
	glm::vec3 camTranslate(0.f);
	glm::vec3 translateDelta(0.f);
	glm::vec2 mouseCur = proto.getNormalizedMouse();
	glm::vec2 mouseLast = proto.getNormalizedMouse();
	
	glm::vec2 translateCur = proto.getNormalizedMouse();
	glm::vec2 translateLast = proto.getNormalizedMouse();
	
	bool isArcDrag = false;
	bool isTranslateDrag = false;
	while( proto.isWindowOpen() ) {
		proto.cls(0,0,0);
		
	  if( proto.keyhit('T') ) {
			puts("toggled two-sided");
			twoSided = !twoSided;
		}
		
		if ( proto.mouseDownLeft() && isArcDrag == false ) {
			isArcDrag = true;
			mouseLast = mouseCur = -1.f + 2.f*proto.getNormalizedMouse();
		}
		if ( proto.mouseDownLeft() && isArcDrag ) {
			mouseCur = -1.f + 2.f*proto.getNormalizedMouse();
		}
		if ( proto.mouseDownLeft()==false && isArcDrag ) {
			isArcDrag = false;
		}
		calcArc( mouseCur, mouseLast, proto.getCamera()->getViewMatrix(), modelMatrix );
		
		glm::vec3 modelXdir = glm::column(modelMatrix,0).xyz();
		glm::vec3 modelYdir = glm::column(modelMatrix,1).xyz();
		glm::vec3 modelZdir = glm::column(modelMatrix,2).xyz();
		
		if ( proto.mouseDownRight() && isTranslateDrag == false ) {
			isTranslateDrag = true;
			translateLast = translateCur = -1.f + 2.f*proto.getNormalizedMouse();
		}
		if ( proto.mouseDownRight() && isTranslateDrag ) {
			translateCur = -1.f + 2.f*proto.getNormalizedMouse();
			
			translateDelta = (glm::vec3(translateCur, 0.f) - glm::vec3(translateLast, 0.f));
			camTranslate = (translateDelta.x+meshTranslate.x) * -proto.getCamera()->getStrafeDirection() + (translateDelta.y+meshTranslate.y) * proto.getCamera()->getUpDirection();
		}
		

		if ( proto.mouseDownRight()==false && isTranslateDrag ) {
			isTranslateDrag = false;
			meshTranslate += translateDelta;
			camTranslate = meshTranslate.x * -proto.getCamera()->getStrafeDirection() + meshTranslate.y * proto.getCamera()->getUpDirection();
		}
		
		proto.getCamera()->lookAt( camTranslate + cameraZoom * dims, camTranslate, glm::vec3(0.f,1.f,0.f) );

		
		if ( proto.keyhit('R') ) {
			puts("reset view");
			camTranslate = glm::vec3(0.f);
			meshTranslate = glm::vec3(0.f);
			proto.setMouseWheel(0);
		}
		
		if ( proto.keyhit('N') ) {
			showNormals = !showNormals;
		}

		if ( proto.keyhit('W') ) {
			proto.toggleWireframe();
		}		
		
		proto.debugNormals( showNormals );
		
		cameraZoom = 1.f + 0.1f * proto.getMouseWheel();
		
		// TRS
		glm::mat4 tform(1.0f);
		tform = glm::translate(tform, -mesh->getCenter() );
		tform *= modelMatrix;
		//tform = glm::scale( tform, meshScale);
		
		proto.setOrientation( tform );
		proto.drawMesh( mesh, twoSided ); // todo, if file not found, give msg box warning, or write logfile... cmd is not enough
		proto.frame();
	}


	puts("program end");
	return 0;
}