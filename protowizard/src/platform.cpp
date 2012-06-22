
#include "proto/platform.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <vector>
#include <algorithm>

void protowizard::ShowMessageBox( const std::string &message, const std::string &title )
{
	MessageBox(0, message.c_str(), title.c_str(), 0);
}

std::string protowizard::GetExePath()
{
	std::vector<char> executablePath(MAX_PATH);

	// Try to get the executable path with a buffer of MAX_PATH characters.
	DWORD result = ::GetModuleFileNameA(
		nullptr, &executablePath[0], static_cast<DWORD>(executablePath.size())
		);

	// As long the function returns the buffer size, it is indicating that the buffer
	// was too small. Keep enlarging the buffer by a factor of 2 until it fits.
	while(result == executablePath.size()) {
		executablePath.resize(executablePath.size() * 2);
		result = ::GetModuleFileNameA(
			nullptr, &executablePath[0], static_cast<DWORD>(executablePath.size())
			);
	}

	// If the function returned 0, something went wrong
	if(result == 0) {
		throw std::runtime_error("GetModuleFileName() failed");
	}

	// We've got the path, construct a standard string from it
	char backslash = '\\';
	char forwardslash = '//';
	std::replace( executablePath.begin(), executablePath.begin() + result, backslash, forwardslash );
	auto path = std::string(executablePath.begin(), executablePath.begin() + result);
	path = path.substr( 0, path.find_last_of('/')+1 );
	return path;
}