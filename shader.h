#pragma once 

#include <string>
#define GLEW_STATIC
#include <glew.h> // for setters funcs in header

class Shader
{
public:
	Shader();

	bool install(const char* VSPath, const char* FSPath);
	bool installFromCString( const char * vertShader, const char * fragShader );
	void reload();
	char* LoadShaderText(const char *fileName);
	void printShaderInfoLog(unsigned int shader);
	void printProgramInfoLog();

	int GetVariable(char* strVariable);
	unsigned int getProgram() { return program; }
	void SetInt(GLint variable, int newValue)								{ glUniform1i(variable, newValue);		}
	void SetFloat(GLint variable, float newValue)							{ glUniform1f(variable, newValue);		}
	void SetFloat2(GLint variable, float v0, float v1)						{ glUniform2f(variable, v0, v1);			}
	void SetFloat3(GLint variable, float v0, float v1, float v2)			{ glUniform3f(variable, v0, v1, v2);		}
	void SetFloat4(GLint variable, float v0, float v1, float v2, float v3)	{ glUniform4f(variable, v0, v1, v2, v3);	}

	void begin();
	void end();
	std::string name;
	
private:
	bool compileSources();
	void validate();
	
	unsigned int program;
	std::string strVSPath;
	std::string strFSPath;
	bool _loadedFromFile;



	std::string _vs_string;
	std::string _fs_string;
};
