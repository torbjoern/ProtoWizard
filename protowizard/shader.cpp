#include "Shader.h"

#include <stdio.h>
#include <iostream>
#include <fstream>


Shader::Shader()
{
	name = "un-initialized shader";
}

Shader::~Shader()
{

}

void Shader::begin() { glUseProgram(program); }
void Shader::end() { glUseProgram(0); }


bool Shader::install(const char* VSPath, const char* FSPath)
{
	name = std::string(VSPath);

	shaderList.push_back( ShaderSource(VSPath, VERTEX_SHADER ) );
	shaderList.push_back( ShaderSource(FSPath, FRAGMENT_SHADER ) );

	if ( compileSources() == false ) {
		return false;
	}

	return true;
}

bool Shader::install(const char* VSPath, const char* GSPath, const char* FSPath)
{
	name = std::string(VSPath);

	shaderList.push_back( ShaderSource(VSPath, VERTEX_SHADER ) );
	shaderList.push_back( ShaderSource(GSPath, GEOMETRY_SHADER ) );
	shaderList.push_back( ShaderSource(FSPath, FRAGMENT_SHADER ) );

	if ( compileSources() == false ) {
		return false;
	}

	return true;
}


bool Shader::compileSources()
{
	ShaderSourceList::iterator it;
	for(it=shaderList.begin(); it!=shaderList.end(); ++it){
		ShaderSource &shader= (*it);

		if ( !shader.hasSourcecode() ) {
			std::cout << "could not compile shader: " << this->name << " missing source" << std::endl;
			return false;
		}
		
		bool compiled = shader.compile();
		if ( !compiled ){
			std::cout << "*** Using fixed function pipeline ***\n";
			std::cout << "--------------------------------\n";
			return false;
		}
	}

	program = glCreateProgram();
	for(it=shaderList.begin(); it!=shaderList.end(); ++it){
		ShaderSource &shader= (*it);
		glAttachShader(program, shader.program);
	}


	for(it=shaderList.begin(); it!=shaderList.end(); ++it){
		ShaderSource &shader= (*it);
		if ( shader.shader_type == GEOMETRY_SHADER ){
			// Expected to be set before linking, src: https://wiki.engr.illinois.edu/display/graphics/Geometry+Shader+Hello+World
			//glProgramParameteriEXT( program, GL_GEOMETRY_INPUT_TYPE_EXT,GL_TRIANGLES); //GL_POINTS/GL_LINES/GL_LINES_ADJACENCY_EXT/GL_TRIANGLES/GL_TRIANGLES_ADJACENCY_EXT
			//glProgramParameteriEXT( program, GL_GEOMETRY_OUTPUT_TYPE_EXT,GL_LINE_STRIP); //GL_POINTS/GL_LINE_STRIP/GL_TRIANGLE_STRIP
			//glProgramParameteriEXT( program, GL_GEOMETRY_VERTICES_OUT_EXT,6); //min max=1024 ?

			glProgramParameteri( program, GL_GEOMETRY_INPUT_TYPE, GL_TRIANGLES); //GL_POINTS/GL_LINES/GL_LINES_ADJACENCY_EXT/GL_TRIANGLES/GL_TRIANGLES_ADJACENCY_EXT
			glProgramParameteri( program, GL_GEOMETRY_OUTPUT_TYPE, GL_LINE_STRIP); //GL_POINTS/GL_LINE_STRIP/GL_TRIANGLE_STRIP
			glProgramParameteri( program, GL_GEOMETRY_VERTICES_OUT, 8); //min max=1024 ?
		}
	}

	
	
	glLinkProgram(program);
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		printShaderInfoLog(program);
		std::cout << "linking of shader failed...\n";
		std::cout << "*** Using fixed function pipeline ***\n";
		std::cout << "--------------------------------\n";
		return false;
	}
	glUseProgram(program); // turn on programmable pipeline
	//std::cout << "Link success...\n";
	//printProgramInfoLog();
	//std::cout << "--------------------------------\n";

	GLenum err = glGetError();
	if ( err == GL_NO_ERROR ){
		const char *status = ":)";
	}else{
		const char *status = ":(";
	}


	return true;
}

bool Shader::reload()
{
	// TODO for_each ShaderSource, re-read from filepath & compile
	ShaderSourceList::iterator it;
	for(it=shaderList.begin(); it!=shaderList.end(); ++it){
		ShaderSource &shader= (*it);
		
		if ( shader.load_sourcefile() == false ){
			return false;
		}
	}

	if ( compileSources() == false ){
		return false;
	}


	bool validation_status = validate();
	return validation_status;
}

void Shader::printShaderInfoLog(unsigned int shader)
{
	int infologLen = 0;
	int charsWritten = 0;
	GLchar* infoLog;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);

	if (infologLen > 0)
	{
		infoLog = (GLchar*) malloc(infologLen);
		if (infoLog == NULL)
		{
			std::cout << "ERROR: Could not allocate infolog buffer" << std::endl;
			exit(1);
		}
		glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog);
		printf("InfoLog shader:\n%s\n", infoLog);
		free( infoLog );
	}
}

bool Shader::validate()
{
	const unsigned int BUFFER_SIZE = 512;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	GLsizei length = 0;

	memset(buffer, 0, BUFFER_SIZE);
	// http://www.opengl.org/sdk/docs/man/xhtml/glGetProgramInfoLog.xml
	glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
	if (length > 0)
		std::cout << "Program " << program << " link error: " << buffer << std::endl;

	// http://www.opengl.org/sdk/docs/man/xhtml/glValidateProgram.xml
	glValidateProgram(program);
	GLint status;
	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE)
	{
		std::cout << "Error validating shader " << program << std::endl;
		return false;
	}
	return true;
}

int Shader::GetVariable( char* strVariable ) const 
{
	int loc = glGetUniformLocation(program, strVariable);
	if ( loc == -1 ){
		// drivers might optimize away uniforms, causing GetUniformLoc to return -1
		fprintf( stderr, "cant find loc for %s in shader %s\n", strVariable, this->name.c_str() ); 

	}
	return loc;
}

char* _loadShaderText(const char *fileName)
{
    char* shaderText = NULL;
    GLint shaderLength = 0;
    FILE* fp;
    fopen_s(&fp, fileName, "r"); // old fopen returns fileptr, fp = fopen(filename,"r")
    if (fp != NULL)
    {
        while (fgetc(fp) != EOF)
        {
            shaderLength++;
        }
        rewind(fp);
        shaderText = (char*)malloc(shaderLength+1);
        if (shaderText != NULL)
        {
			fread(shaderText, 1, shaderLength, fp);
        }
        shaderText[shaderLength] = '\0';
        fclose(fp);
    }
    return shaderText;
}

ShaderSource::ShaderSource(std::string _sourcefile, int _shader_type){
	sourcefile = _sourcefile;
	shader_type = _shader_type;

	if ( load_sourcefile() )
	{
		if ( shader_type == VERTEX_SHADER )
		{
			program = glCreateShader(GL_VERTEX_SHADER);
		}else if( shader_type == GEOMETRY_SHADER ){
			program = glCreateShader(GL_GEOMETRY_SHADER);
		}
		else if( shader_type == FRAGMENT_SHADER ){
			program = glCreateShader(GL_FRAGMENT_SHADER);
		}
	}
	
}

bool ShaderSource::load_sourcefile()
{
	char* temp = _loadShaderText( sourcefile.c_str() );
	if( temp == NULL)
	{
		std::cerr << "error, cant find shader file: " << sourcefile << std::endl;
		return false;
	}
	sourcecode = std::string(temp);
	free( temp );
	return true;
}

bool ShaderSource::compile()
{	
	const char *src = sourcecode.c_str();
	glShaderSource(program, 1, &src, NULL);

	glCompileShader(program);
	GLint compile_status;
	glGetShaderiv(program, GL_COMPILE_STATUS, &compile_status);
	if ( !compile_status )
	{
		std::cout << "error when compiling shader: " << sourcefile << "\n\n";
		Shader::printShaderInfoLog(program);
		return false;
	}

	return true;
}