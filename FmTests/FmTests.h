#pragma once

#define _WIN32_WINNT 0x0400	// Change this to the appropriate value to target other versions of Windows.
						
#include <windows.h>
#include <stdio.h>
#include <locale.h>

#include <Winnetwk.h>

#pragma comment (lib, "Mpr.lib")

BOOL WINAPI EnumerateFunc (NETRESOURCE* lpnr);





