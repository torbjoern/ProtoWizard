#pragma once;
#include <string>

class MeshManager
{
public:
	virtual ~MeshManager() {}
	virtual void drawMesh( const std::string& file_path ) = 0;

	static MeshManager *init();
	static void shutdown(MeshManager *tm);
};
