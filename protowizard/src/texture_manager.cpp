#include "proto/texture_manager.h"

#include "../depends/soil/SOIL.h"

#include "proto/common.h"
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
private:
	typedef std::shared_ptr<TextureHandle> TextureHandlePtr;
	std::map<std::string, TextureHandlePtr> texture_handle_map;
	unsigned int active_texture;
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

	TextureHandlePtr loadTexture( const std::string& file_path )
	{
		// SOIL_load_OGL_texture requires a const char*, and isnt happy with file_path.c_str
		std::vector<char> cstr_path(file_path.size() + 1);
		std::copy(file_path.begin(), file_path.end(), cstr_path.begin());

		GLuint texture_handle = 0;
		glGenTextures( 1, &texture_handle );
		texture_handle = SOIL_load_OGL_texture( &cstr_path[0], SOIL_LOAD_AUTO, texture_handle, SOIL_FLAG_MIPMAPS|SOIL_FLAG_MULTIPLY_ALPHA|SOIL_FLAG_INVERT_Y ); // SOIL_FLAG_INVERT_Y kan være nyttig 
		GetError( "TextureManager::setTexture" );

		if ( texture_handle == 0 )
		{
			throw std::runtime_error("failed to load texture " + file_path);
		} else {
			texture_handle_map[file_path] = std::make_shared<TextureHandle>( file_path, texture_handle ); 
		}

		return texture_handle_map[file_path];
	}

	virtual void setTexture( const std::string& file_path )
	{
		auto it = texture_handle_map.find( file_path );
		if ( it == texture_handle_map.end() )
		{
			texture_handle_map[file_path] = loadTexture( file_path );
		} 
		active_texture = texture_handle_map[file_path]->tex_handle;
	}

	virtual unsigned int getActiveTexture(){ return active_texture; }

};

TextureManager *TextureManager::init() {
	return new TextureManagerImplementation;
}

void TextureManager::shutdown(TextureManager *tm) {
	delete tm;
}