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
	TextureManager();
	void shutdown();
	void disableTextures() { active_texture.tex_handle = 0; }
	void setTexture( const std::string& file_path );
	unsigned int getActiveTexture(){ return active_texture.tex_handle; }
private:
	std::map<std::string, TextureHandle*> texture_handle_map;
	TextureHandle active_texture;
};
