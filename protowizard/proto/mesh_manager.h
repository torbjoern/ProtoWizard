#pragma once;
#include <string>
#include <memory>

namespace protowizard{
	class Mesh;
	typedef std::shared_ptr<Mesh> MeshPtr;

	namespace MeshManager
	{
		MeshPtr getMesh( const std::string& file_path );
		void shutdown();
	};
}
