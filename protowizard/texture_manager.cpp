#include "texture_manager.h"

#include "../depends/soil/SOIL.h"

#include "common.h"
#include <map>
#include <memory>

class TextureHandle
{
public:

	TextureHandle() : tex_handle(0)
	{
	}

	TextureHandle( const std::string& path, unsigned int tex_handle ) : path(path), tex_handle(tex_handle)
	{
	}

	~TextureHandle() {
		glDeleteTextures( 1, &tex_handle );
	}

	unsigned int tex_handle;
	std::string path;
};

class TextureManagerImplementation : public TextureManager
{
	friend class TextureManager;
public:
	TextureManagerImplementation() {
		active_texture = 0;
	}
	
	virtual ~TextureManagerImplementation() 
	{
		size_t num_items = texture_handle_map.size();
		texture_handle_map.clear();
	}
	
	virtual void disableTextures() { active_texture = 0; }

	virtual void setTexture( const std::string& file_path )
	{
		using std::string;

		std::map<string, TextureHandlePtr>::iterator it;
	
		it = texture_handle_map.find( file_path );
	
		if ( it == texture_handle_map.end() )
		{
			const int buf_len = file_path.length()+1;
			char* cstr_path = new char[buf_len];
			strcpy_s( cstr_path, buf_len, file_path.c_str() );

			GLuint texture_handle = 0;
			glGenTextures( 1, &texture_handle );
			texture_handle = SOIL_load_OGL_texture(cstr_path, SOIL_LOAD_AUTO, texture_handle, SOIL_FLAG_MIPMAPS|SOIL_FLAG_MULTIPLY_ALPHA|SOIL_FLAG_INVERT_Y ); // SOIL_FLAG_INVERT_Y kan være nyttig 
			//glActiveTexture(GL_TEXTURE0 + 0); // Texture Unit to use
			glBindTexture(GL_TEXTURE_2D, texture_handle);
		
			//int success = glfwLoadTexture2D(cstr_path, GLFW_BUILD_MIPMAPS_BIT); // GLFW_BUILD_MIPMAPS_BIT
			//// Use trilinear interpolation for minification
			//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
			//// Use bilinear interpolation for magnification
			//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

			if ( texture_handle == 0 )
			{
				printf("failed to load texture %s...\n", file_path);
				// use place holder texture generated in memory
			}
			texture_handle_map[file_path] = std::make_shared<TextureHandle>( file_path, texture_handle ); 

			GetError( "TextureManager::setTexture" );
		} 

		active_texture = texture_handle_map[file_path]->tex_handle;
	}

	virtual unsigned int getActiveTexture(){ return active_texture; }
private:
	typedef std::shared_ptr<TextureHandle> TextureHandlePtr;
	std::map<std::string, TextureHandlePtr> texture_handle_map;
	unsigned int active_texture;
};

TextureManager *TextureManager::init() {
	return new TextureManagerImplementation;
}

void TextureManager::shutdown(TextureManager *tm) {
	delete tm;
}