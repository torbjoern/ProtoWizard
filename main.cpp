#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>

#include "protographics.h"

#include <string>
using std::string;
#include <cassert>
#include <cstdio>
#include <fstream>

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

	// Register the function that we want the scripts to call 
	r = (*engine)->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL); assert( r >= 0 );
	r = (*engine)->RegisterGlobalFunction("void cls(float r, float g, float b)", asFUNCTION(cls), asCALL_CDECL); assert( r >= 0 );
	r = (*engine)->RegisterGlobalFunction("void circle(float x, float y, float r)", asFUNCTION(circle), asCALL_CDECL); assert( r >= 0 );

	// Create our context, prepare it, and then execute
	*ctx = (*engine)->CreateContext();

	// The CScriptBuilder helper is an add-on that loads the file,
	// performs a pre-processing pass if necessary, and then tells
	// the engine to build a script module.
	CScriptBuilder builder;
	r = builder.StartNewModule(*engine, "MyModule"); 
	if( r < 0 ) 
	{
		// If the code fails here it is usually because there
		// is no more memory to allocate the module
		printf("Unrecoverable error while starting a new module.\n");
		return 0;
	}
	r =  builder.AddSectionFromFile("test.as");
	if( r < 0 )
	{
		// The builder wasn't able to load the file. Maybe the file
		// has been removed, or the wrong name was given, or some
		// preprocessing commands are incorrectly written.
		printf("Please correct the errors in the script and try again.\n");
		return 0 ;
	}
	r = builder.BuildModule();
	if( r < 0 )
	{
		// An error occurred. Instruct the script writer to fix the 
		// compilation errors that were listed in the output stream.
		printf("Please correct the errors in the script and try again.\n");
		return 0;
	}


	// Find the function that is to be called. 
	asIScriptModule *mod = (*engine)->GetModule("MyModule");

	funcId = mod->GetFunctionIdByDecl("void main(int mousx, int mousy)");
	if( funcId < 0 )
	{
		// The function couldn't be found. Instruct the script writer
		// to include the expected function in the script.
		printf("The script must have the function 'void main()'. Please add it and try again.\n");
		return 0;
	}


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

	if ( protoGraphics.init() == false )
	{
		printf("Failed to init OpenGL graphics\n");
		return 0;
	}



	while( true )
	{
		// pass main(mousx, mousy) its parameters and then call it
		ctx->Prepare(funcId);
		ctx->SetArgDWord(0, protoGraphics.getMouseX());
		ctx->SetArgDWord(1, protoGraphics.getMouseY());

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

		protoGraphics.frame();
		

		if (protoGraphics.isWindowOpen() == false) break;
	}

	protoGraphics.shutdown();



	// Clean up
	ctx->Release();
	engine->Release();
}