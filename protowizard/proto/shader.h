#pragma once 

#include <string>
#include "opengl_stuff.h"

#include <glm/glm.hpp>

#include <vector>
#include <algorithm>

enum eShaderTypes { VERTEX_SHADER, GEOMETRY_SHADER, FRAGMENT_SHADER };


class ShaderSource
{
public:
	explicit ShaderSource(std::string _sourcefile, int _shader_type);

	bool compile();
	bool load_sourcefile();

	bool hasSourcecode() { return sourcecode.length() > 0; }

public:
	int program;
	int shader_type;
private:
	std::string sourcefile;
	std::string sourcecode;
};

class Shader
{
public:
	Shader();
	Shader(const std::string& vert, const std::string& frag);
	Shader(const std::string& vert, const std::string& geo, const std::string& frag);
	~Shader();

	void shutdown()
	{
		// http://www.opengl.org/sdk/docs/man/xhtml/glDeleteProgram.xml
		glDeleteProgram( program );

		// http://www.opengl.org/sdk/docs/man/xhtml/glDeleteShader.xml
		for(auto it=shaderList.begin(); it!=shaderList.end(); ++it){
			glDeleteShader( (*it).program );
		}
	}

	bool load();
	bool reload();

	static void printShaderInfoLog(unsigned int shader);

	int GetVariable(char* strVariable) const;
	unsigned int getProgram() const { return program; }
	void SetInt(GLint variable, int newValue) const									{ glUniform1i(variable, newValue);			}
	void SetFloat(GLint variable, float newValue) const								{ glUniform1f(variable, newValue);			}
	void SetFloat2(GLint variable, float v0, float v1) const						{ glUniform2f(variable, v0, v1);			}
	void SetFloat3(GLint variable, float v0, float v1, float v2) const				{ glUniform3f(variable, v0, v1, v2);		}
	void SetFloat4(GLint variable, float v0, float v1, float v2, float v3) const	{ glUniform4f(variable, v0, v1, v2, v3);	}
	void SetVec3(GLint variable, glm::vec3 const& v) const							{ glUniform3f(variable, v.x, v.y, v.z);		}
	void SetVec4(GLint variable, glm::vec4 &v) const								{ glUniform4f(variable, v.x, v.y, v.z, v.a); }

	void begin();
	void end();
	std::string name;
	
private:
	bool compileSources();
	bool validate();

private:	
	unsigned int program;
	typedef std::vector< ShaderSource > ShaderSourceList;
	ShaderSourceList shaderList;

	std::string vsPath;
	std::string gsPath;
	std::string fsPath;
};
