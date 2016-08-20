// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define DIRECTINPUT_VERSION 0x0800
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <d3d9.h>
#include <dinput.h>

#define CHECK_DX_ERROR if (result != D3D_OK) { printf("ERROR DX %d | @ %s : %d", result, __FILE__, __LINE__); }

#include "detours.h"
#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\d3dx9.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

//#pragma comment(lib, "detours.lib")
#include "resource.h"


#include "VariableStorage.h"

#include "DrawableRectangle.h"
#include "TexturedRectangle.h"

#include "HookingNamespace.h"


// TODO: reference additional headers your program requires here
