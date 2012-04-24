#pragma once;
#include <string>

class TextureManager
{
public:
	virtual ~TextureManager() {}
	virtual void disableTextures() = 0;
	virtual void setTexture( const std::string& file_path ) = 0;
	virtual unsigned int getActiveTexture() = 0;

	static TextureManager *init();
	static void shutdown(TextureManager *tm);
};
