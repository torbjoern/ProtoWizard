
#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptmath/scriptmath.h>

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
inline int getMouseX(){
	return protoGraphics.getMouseX();
}
inline int getMouseY(){
	return protoGraphics.getMouseY();
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

bool init( asIScriptEngine **engine, asIScriptContext **ctx, int &funcId )
{
	// Create the script engine
	*engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

	// Set the message callback to receive information on errors in human readable form.
	int r = (*engine)->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL); assert( r >= 0 );

	// AngelScript doesn't have a built-in string type, as there is no definite standard 
	// string type for C++ applications. Every developer is free to register it's own string type.
	// The SDK do however provide a standard add-on for registering a string type, so it's not
	// necessary to implement the registration yourself if you don't want to.
	RegisterStdString(*engine);

	RegisterScriptMath(*engine);

	// Register the function that we want the scripts to call 
	r = (*engine)->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL); assert( r >= 0 );
	r = (*engine)->RegisterGlobalFunction("void cls(float r, float g, float b)", asFUNCTION(cls), asCALL_CDECL); assert( r >= 0 );
	r = (*engine)->RegisterGlobalFunction("void circle(float x, float y, float r)", asFUNCTION(circle), asCALL_CDECL); assert( r >= 0 );
	r = (*engine)->RegisterGlobalFunction("void moveto(float x, float y)", asFUNCTION(moveTo), asCALL_CDECL); assert( r >= 0 );
	r = (*engine)->RegisterGlobalFunction("void lineto(float x, float y)", asFUNCTION(lineTo), asCALL_CDECL); assert( r >= 0 );
	r = (*engine)->RegisterGlobalFunction("void setcol(float r, float g, float b)", asFUNCTION(setcol), asCALL_CDECL); assert( r >= 0 );
	
	r = (*engine)->RegisterGlobalFunction("int mousx()", asFUNCTION(getMouseX), asCALL_CDECL); assert( r >= 0 );
	r = (*engine)->RegisterGlobalFunction("int mousy()", asFUNCTION(getMouseY), asCALL_CDECL); assert( r >= 0 );


	// Create our context, prepare it, and then execute
	*ctx = (*engine)->CreateContext();

	return true;
}

bool reload_script(asIScriptEngine *&engine, int &funcId){
	
	if (load_and_compile(&engine) == false ) return false;
	
	if (start_script_main(&engine, funcId) == false ) return false;
	
	return true;
}

int main()
{
	asIScriptEngine *engine = NULL;
	asIScriptContext *ctx = NULL;

	int funcId = 0;
	if ( init( &engine, &ctx, funcId ) == false )
	{
		printf("failed to init angelscript\n");
		return 0;
	}

	if ( protoGraphics.init(800,600) == false )
	{
		printf("Failed to init OpenGL graphics\n");
		return 0;
	}

	bool script_compiled = reload_script(engine, funcId);

	while( true )
	{
		if ( script_compiled == false ){
			protoGraphics.cls(1.f, 1.f, 1.f);
			protoGraphics.setTitle("Script has errors");
		} else {
			protoGraphics.setTitle("Script running");

			// pass main(mousx, mousy) its parameters and then call it
			ctx->Prepare(funcId);
			//ctx->SetArgDWord(0, protoGraphics.getMouseX());
			//ctx->SetArgDWord(1, protoGraphics.getMouseY());

			int r = ctx->Execute();


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

		bool can_reload = true;
		if ( protoGraphics.keystatus('S') && can_reload ){
			can_reload = false;
			printf("ScriptManager: Reloading from disk\n");

			script_compiled = reload_script(engine, funcId);
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