#include "Shader.h"

#include <stdio.h>
#include <iostream>
#include <fstream>


Shader::Shader()
{
	name = "un-initialized shader";
}

void Shader::begin() { glUseProgram(program); }
void Shader::end() { glUseProgram(0); }

bool Shader::installFromCString( const char * vertShader, const char * fragShader )
{
	_loadedFromFile = false;

	// TODO should copy source bytes into its own buffer... as they may dissapear
	_vs_string = std::string(vertShader);
	_fs_string = std::string(fragShader);

	return compileSources();
}

bool Shader::install(const char* VSPath, const char* FSPath)
{
	_loadedFromFile = true;
	name = std::string(VSPath);

	this->strVSPath = std::string(VSPath);
	this->strFSPath = std::string(FSPath);
	char* temp = LoadShaderText(VSPath);
	if( temp == NULL)
	{
		std::cout << "error, cant find shader file: " << VSPath << "\n\n";
		return false;
	}

	_vs_string = std::string(temp);
	temp = LoadShaderText(FSPath);
	if( temp == NULL)
	{
		std::cout << "error, cant find shader file: " << FSPath << "\n\n";
		return false;
	}

	_fs_string = std::string(temp);
	temp = 0;

	if ( compileSources() == false )
	{
		return false;
	}

	return true;
}

bool Shader::compileSources()
{
	GLuint VS, FS;
	GLint VSCompiled, FSCompiled, linked;

	VS = glCreateShader(GL_VERTEX_SHADER);
	FS = glCreateShader(GL_FRAGMENT_SHADER);

	const char *vs_src = _vs_string.c_str();
	const char *fs_src = _fs_string.c_str();
	glShaderSource(VS, 1, &vs_src, NULL);
	glShaderSource(FS, 1, &fs_src, NULL);

	glCompileShader(VS);
	glGetShaderiv(VS, GL_COMPILE_STATUS, &VSCompiled);
	if ( !VSCompiled )
	{
		std::cout << "error when compiling vertex shader: " << strVSPath << "\n\n";
		printShaderInfoLog(VS);
	}
	//else{
	//	std::cout << "vertex shader compiled: " << strVSPath << "\n";
	//}
	glCompileShader(FS);
	glGetShaderiv(FS, GL_COMPILE_STATUS, &FSCompiled);
	if ( !FSCompiled )
	{
		std::cout << "error when compiling fragment shader: " << strFSPath << "\n\n";
		printShaderInfoLog(FS);
	}
	//else 
	//	std::cout << "Fragment shader compiled: " << strFSPath << "\n";

	if (!VSCompiled || !FSCompiled)
	{
		std::cout << "*** Using fixed function pipeline ***\n";
		std::cout << "--------------------------------\n";
		return false;
	}

	program = glCreateProgram();
	glAttachShader(program, VS);
	glAttachShader(program, FS);

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		printProgramInfoLog();
		std::cout << "linking of shader failed...\n";
		std::cout << "*** Using fixed function pipeline ***\n";
		std::cout << "--------------------------------\n";
		return false;
	}
	glUseProgram(program); // turn on programmable pipeline
	//std::cout << "Link success...\n";
	//printProgramInfoLog();
	//std::cout << "--------------------------------\n";
	return true;
}

void Shader::reload()
{
	if( _loadedFromFile )
	{
		this->install( strVSPath.c_str(), strFSPath.c_str() );
	}else
	{
		printf("Shader source is in memory and cannot be reloaded from disk.\n");
	}

	validate();
	
}

char* Shader::LoadShaderText(const char *fileName)
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
	}
}

void Shader::printProgramInfoLog()
{
	int infologLen = 0;
	int charsWritten = 0;
	GLchar* infoLog;

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLen);

	if (infologLen > 1)
	{
		infoLog = (GLchar*) malloc(infologLen);
		if (infoLog == NULL)
		{
			std::cout << "ERROR: Could not allocate infolog buffer" << std::endl;
			exit(1);
		}
		glGetProgramInfoLog(program, infologLen, &charsWritten, infoLog);
		printf("InfoLog program:\n%s\n", infoLog);
	}
}

void Shader::validate()
{
	const unsigned int BUFFER_SIZE = 512;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	GLsizei length = 0;

	memset(buffer, 0, BUFFER_SIZE);
	glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
	if (length > 0)
		std::cout << "Program " << program << " link error: " << buffer << std::endl;

	glValidateProgram(program);
	GLint status;
	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE)
		std::cout << "Error validating shader " << program << std::endl;
}

int Shader::GetVariable( char* strVariable )
{
	return glGetUniformLocation(program, strVariable);
}
