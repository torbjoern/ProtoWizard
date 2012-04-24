#include "path.h"

// expects somedir/bin/debug/program.exe
std::string extractExePath( std::string & path )
{
	size_t found = path.find_last_of("/\\");
	//std::string exe_name = path.substr( found+1 );
	std::string folder_with_debug = path.substr( 0, found ); // gives somedir/bin/debug

	size_t found_debug = folder_with_debug.find_last_of("/\\"); // expects somedir/bin/debug
	std::string folder_only = folder_with_debug.substr( 0, found_debug ); // gives somedir/bin
	return folder_only;
}
