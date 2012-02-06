#include <map>
#include <string>


class TextureHandle
{
public:

	TextureHandle() : tex_handle(0)
	{
	}

	TextureHandle( unsigned int handle_ ) : tex_handle(handle_)
	{
	}

	~TextureHandle();

	unsigned int tex_handle;
};

class TextureManager
{
public:
	static void setTexture( const std::string& file_path );
	static unsigned int getActiveTexture(){ return active_texture->tex_handle; }
private:
	static std::map<std::string, TextureHandle*> texture_handle_map;
	static TextureHandle *active_texture;
};
