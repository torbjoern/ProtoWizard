#ifndef _PLATFORM_H
#define _PLATFORM_H

#include "platform.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void Show_message_box( std::string str )
{
	MessageBox(0, str.c_str(), "torbjoern cant code", 0);
}

#endif



