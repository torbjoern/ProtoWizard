#pragma comment (linker, "/NODEFAULTLIB:LIBCMT.lib") 

#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptmath/scriptmath.h>
#include <scriptarray/scriptarray.h>

#include "protographics.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp> // Value ptr -> column-ordered pointer to GLM type

#include <string>
using std::string;
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>

// TODO: fix evil global
ProtoGraphics protoGraphics;

void print(string &msg)
{
	printf("%s", msg.c_str());
}

void cls(float r, float g, float b)
{
	protoGraphics.cls(r,g,b);
}

void circle(float x, float y, float r)
{
	protoGraphics.drawCircle(x,y,r);
}

inline void moveTo(float x, float y) {
	protoGraphics.moveTo(x,y);
}
inline void lineTo(float x, float y) {
	protoGraphics.lineTo(x,y);
}
inline void setcol(float r, float g, float b) {
	protoGraphics.setColor(r,g,b);
}
inline void setalpha(float a) {
	protoGraphics.setAlpha(a);
}
inline void enableLightblend() {
	protoGraphics.setBlend(true);
}
inline void disableLightblend() {
	protoGraphics.setBlend(false);
}


inline int getMouseX(){
	return protoGraphics.getMouseX();
}
inline int getMouseY(){
	return protoGraphics.getMouseY();
}
inline bool mouseDownLeft(){
	return protoGraphics.mouseDownLeft();
}
inline bool mouseDownRight(){
	return protoGraphics.mouseDownRight();
}
inline bool keystatus(int key){
	return protoGraphics.keystatus(key);
}
inline bool keyhit(int key){
	return protoGraphics.keyhit(key);
}


inline void drawsph(float x, float y, float z, float r){
	protoGraphics.drawSphere( glm::vec3(x,y,z), r );
}
inline void drawcube(float x, float y, float z, float r){
	protoGraphics.drawCube( glm::vec3(x,y,z), r );
}
inline void drawcone(float x1, float y1, float z1, 
					 float x2, float y2, float z2,
					 float r){
	protoGraphics.drawCone( glm::vec3(x1,y1,z1), glm::vec3(x2,y2,z2), r  );
}


inline void setcam(float x, float y, float z, float hang, float vang){
	protoGraphics.setCamera( x,y,z, hang, vang );
}
inline void wasdcam(bool leftkey, bool rightkey, bool backkey, bool fwdkey, int mousx, int mousy, bool mousedown, float delta){
	FirstPersonCamera &cam = protoGraphics.getCamera();
	cam.update( leftkey, rightkey, backkey, fwdkey, mousx, mousy, mousedown, delta );
}
inline void setRotation( float xx, float xy, float xz,
						 float yx, float yy, float yz,
						 float zx, float zy, float zz )
{
	glm::mat4 ori(0.0);
	ori[0] = glm::vec4( xx, xy, xz, 0.0f );
	ori[1] = glm::vec4( yx, yy, yz, 0.0f );
	ori[2] = glm::vec4( zx, zy, zz, 0.0f );
	ori[3] = glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	protoGraphics.setOrientation( ori );
}


// Implement a simple message callback function
void MessageCallback(const asSMessageInfo *msg, void *param)
{
	const char *type = "ERR ";
	if( msg->type == asMSGTYPE_WARNING ) 
		type = "WARN";
	else if( msg->type == asMSGTYPE_INFORMATION ) 
		type = "INFO";
	printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

bool load_and_compile(asIScriptEngine **engine)
{
	// The CScriptBuilder helper is an add-on that loads the file,
	// performs a pre-processing pass if necessary, and then tells
	// the engine to build a script module.
	CScriptBuilder builder;
	int r = 0;
	r = builder.StartNewModule(*engine, "MyModule"); 
	if( r < 0 ) 
	{
		// If the code fails here it is usually because there
		// is no more memory to allocate the module
		printf("Unrecoverable error while starting a new module.\n");
		return false;
	}

	r =  builder.AddSectionFromFile("test.as");
	if( r < 0 )
	{
		// The builder wasn't able to load the file. Maybe the file
		// has been removed, or the wrong name was given, or some
		// preprocessing commands are incorrectly written.
		printf("Please correct the errors in the script and try again.\n");
		return false;
	}
	r = builder.BuildModule();
	if( r < 0 )
	{
		// An error occurred. Instruct the script writer to fix the 
		// compilation errors that were listed in the output stream.
		printf("Please correct the errors in the script and try again.\n");
		return false;
	}

	return true;
}

bool start_script_main(asIScriptEngine **engine, int &funcId){
	// Find the function that is to be called. 
	asIScriptModule *mod = (*engine)->GetModule("MyModule");

	std::string main_function = "void onFrame()";
	funcId = mod->GetFunctionIdByDecl(main_function.c_str());
	if( funcId < 0 )
	{
		// The function couldn't be found. Instruct the script writer
		// to include the expected function in the script.
		std::cout << "The script must have the function " << main_function << ". Please add it and try again." << std::endl;
		return false;
	}
	return true;
}

bool init( asIScriptEngine *&engine, asIScriptContext *&ctx, int &funcId )
{
	// Create the script engine
	engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

	// Set the message callback to receive information on errors in human readable form.
	int r = (engine)->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL); assert( r >= 0 );

	// AngelScript doesn't have a built-in string type, as there is no definite standard 
	// string type for C++ applications. Every developer is free to register it's own string type.
	// The SDK do however provide a standard add-on for registering a string type, so it's not
	// necessary to implement the registration yourself if you don't want to.
	RegisterStdString(engine);

	RegisterScriptMath(engine);

	RegisterScriptArray(engine, false);

	// Register the function that we want the scripts to call 
	r = (engine)->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL); assert( r >= 0 );
	r = (engine)->RegisterGlobalFunction("void cls(float r, float g, float b)", asFUNCTION(cls), asCALL_CDECL); assert( r >= 0 );
	r = (engine)->RegisterGlobalFunction("void circle(float x, float y, float r)", asFUNCTION(circle), asCALL_CDECL); assert( r >= 0 );
	r = (engine)->RegisterGlobalFunction("void moveto(float x, float y)", asFUNCTION(moveTo), asCALL_CDECL); assert( r >= 0 );
	r = (engine)->RegisterGlobalFunction("void lineto(float x, float y)", asFUNCTION(lineTo), asCALL_CDECL); assert( r >= 0 );
	r = (engine)->RegisterGlobalFunction("void setcol(float r, float g, float b)", asFUNCTION(setcol), asCALL_CDECL); assert( r >= 0 );
	r = (engine)->RegisterGlobalFunction("void setalpha(float a)", asFUNCTION(setalpha), asCALL_CDECL); assert( r >= 0 );
	r = (engine)->RegisterGlobalFunction("void enableLightblend()", asFUNCTION(enableLightblend), asCALL_CDECL); assert( r >= 0 );
	r = (engine)->RegisterGlobalFunction("void disableLightblend()", asFUNCTION(disableLightblend), asCALL_CDECL); assert( r >= 0 );
	r = (engine)->RegisterGlobalFunction("void drawsph(float x, float y, float z, float r)", asFUNCTION(drawsph), asCALL_CDECL); assert( r >= 0 );
	r = (engine)->RegisterGlobalFunction("void drawcube(float x, float y, float z, float r)", asFUNCTION(drawcube), asCALL_CDECL); assert( r >= 0 );
	r = (engine)->RegisterGlobalFunction("void 	drawcone(float x1, float y1, float z1, float x2, float y2, float z2, float r)", asFUNCTION(drawcone), asCALL_CDECL); assert( r >= 0 );



	r = (engine)->RegisterGlobalFunction("void setcam(float x, float y, float z, float hang, float vang)", asFUNCTION(setcam), asCALL_CDECL); assert( r >= 0 );
	r = (engine)->RegisterGlobalFunction("void wasdcam(bool leftkey, bool rightkey, bool backkey, bool fwdkey, int mousx, int mousy, bool mousedown, float delta)", asFUNCTION(wasdcam), asCALL_CDECL); assert( r >= 0 );
	
	r = (engine)->RegisterGlobalFunction("void setRotation( float xx, float xy, float xz, "
															"float yx, float yy, float yz,"
															"float zx, float zy, float zz )" , asFUNCTION(setRotation), asCALL_CDECL); assert( r >= 0 );
	

	r = (engine)->RegisterGlobalFunction("int getMouseX()", asFUNCTION(getMouseX), asCALL_CDECL); assert( r >= 0 );
	r = (engine)->RegisterGlobalFunction("int getMouseY()", asFUNCTION(getMouseY), asCALL_CDECL); assert( r >= 0 );
	r = (engine)->RegisterGlobalFunction("bool mouseLeft()", asFUNCTION(mouseDownLeft), asCALL_CDECL); assert( r >= 0 );
	r = (engine)->RegisterGlobalFunction("bool mouseRight()", asFUNCTION(mouseDownRight), asCALL_CDECL); assert( r >= 0 );
	r = (engine)->RegisterGlobalFunction("bool keystatus(int key)", asFUNCTION(keystatus), asCALL_CDECL); assert( r >= 0 );
	r = (engine)->RegisterGlobalFunction("bool keyhit(int key)", asFUNCTION(keyhit), asCALL_CDECL); assert( r >= 0 );


	// Create our context, prepare it, and then execute
	ctx = (engine)->CreateContext();

	return true;
}

bool reload_script(asIScriptEngine **engine, int &funcId){
	if (load_and_compile(engine) == false ) return false;
	
	if (start_script_main(engine, funcId) == false ) return false;
	
	return true;
}

void cpp()
{
	cls(0,0,0);

	setcol(255,255,255);

	float xres = 600;
	float a = float(getMouseX() / xres ) * TWO_PI;
	float ca = cos(a);
	float sa = sin(a);
	float dist = -70.0;

	setcam( sa*dist,0, ca*dist, -a, 0 );

	//enableLightblend();
	disableLightblend();
	setalpha(0.7);

	int num = 12;
	float spacing = 4.0;

	//setcol(0.8, 0.8, 0.8);

	for(int i=0; i< num; i++){
		for(int j=0; j< num; j++)
			for(int k=0; k< num; k++)
			{
				setcol( i/float(num), j/float(num), k/float(num) );
				drawsph(-spacing*(num-1)*0.5 + i*1*spacing, 
					-spacing*(num-1)*0.5 + j*1*spacing, 
					-spacing*(num-1)*0.5 + k*1*spacing, 1.5);
			}
	}

}

int main()
{
	asIScriptEngine *engine = NULL;
	asIScriptContext *ctx = NULL;

	int funcId = 0;
	if ( init( engine, ctx, funcId ) == false )
	{
		printf("failed to init angelscript\n");
		return 0;
	}


	if ( protoGraphics.init(800,600) == false )
	{
		printf("Failed to init OpenGL graphics\n");
		return 0;
	}

	bool script_compiled = reload_script(&engine, funcId);

	float script_mspf = 0.0f;

	while( true )
	{
		if ( script_compiled == false ){
			protoGraphics.cls(1.f, 1.f, 1.f);
			protoGraphics.setTitle("Script has errors");
		} else {
			

			double perf_start = protoGraphics.klock();
			ctx->Prepare(funcId);
			int r = ctx->Execute();
			//cpp();
			script_mspf = protoGraphics.klock() - perf_start;


			if( r != asEXECUTION_FINISHED )
			{
				// The execution didn't complete as expected. Determine what happened.
				if( r == asEXECUTION_EXCEPTION )
				{
					// An exception occurred, let the script writer know what happened so it can be corrected.
					printf("An exception '%s' occurred. Please correct the code and try again.\n", ctx->GetExceptionString());
				}
			}
		}

		protoGraphics.frame();

		float mspf = protoGraphics.getMSPF();
		
		char title_buf[256];
		sprintf_s(title_buf, 256, "render mspf: %.1f mspf, script mspf: = %.f", mspf*1000.0f, script_mspf * 1000.0f);
		protoGraphics.setTitle( std::string(title_buf) );

		bool can_reload = true;
		if ( protoGraphics.keystatus('S') && can_reload ){
			can_reload = false;
			printf("ScriptManager: Reloading from disk\n");

			script_compiled = reload_script(&engine, funcId);
		}
		if ( protoGraphics.keystatus('S') == false ){
			can_reload = true;
		}




		if (protoGraphics.isWindowOpen() == false) break;
	}



	// Clean up
	ctx->Release();
	engine->Release();
}