
#include "proto/platform.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <CommDlg.h>

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
	std::replace( executablePath.begin(), executablePath.begin() + (int)result, backslash, forwardslash );
	auto path = std::string(executablePath.begin(), executablePath.begin() + result);
	path = path.substr( 0, path.find_last_of('/')+1 );
	return path;
}

std::string protowizard::OpenFileDlg(const std::string &path, const std::string &extension)
{
	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "";

	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = 0x0;//lwnd;

	const std::string fileFilter =  "*." + extension;
	//ofn.lpstrFilter = "*.obj";//fileFilter.c_str();
	ofn.lpstrFilter = "obj 3d file\0*.obj;       \0All Files (*.*)      \0*.*       \0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER|OFN_FILEMUSTEXIST;
	ofn.lpstrDefExt = extension.c_str();
	if (GetOpenFileName(&ofn))
	{
		return std::string(ofn.lpstrFile);
	}
	return std::string("");
}
