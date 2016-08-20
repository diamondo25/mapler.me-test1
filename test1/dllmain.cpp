// #define _HKDEBUG_

typedef HRESULT (WINAPI *CreateDevice_t)(IDirect3D9* Direct3D_Object, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, 
                    DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, 
                    IDirect3DDevice9** ppReturnedDeviceInterface);
typedef HRESULT (WINAPI *EndScene_t)(IDirect3DDevice9* surface);
typedef HRESULT (WINAPI *Reset_t)(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);


CreateDevice_t D3DCreateDevice_orig;
EndScene_t D3DEndScene_orig;
Reset_t D3DReset_orig;

PDWORD IDirect3D9_vtable = NULL;
LPDIRECT3DDEVICE9 npDevice;

#define CREATEDEVICE_VTI 16
#define ENDSCENE_VTI 42
#define RESET_VTI 16

HRESULT WINAPI HookCreateDevice();
DWORD WINAPI VTablePatchThread(LPVOID threadParam);
HRESULT WINAPI D3DEndScene_hook(IDirect3DDevice9* device);
HRESULT WINAPI D3DReset_hook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);


DWORD WINAPI VTablePatchThread(LPVOID threadParam)
{
#ifdef _HKDEBUG_
	MessageBoxA(NULL, "VTable patch thread started", "Patch Thread", MB_ICONEXCLAMATION);
#endif
	std::cout << "VTable patch thread started" << std::endl;
	while (true)
	{
		Sleep(100);
		if (*(DWORD*)&IDirect3D9_vtable[ENDSCENE_VTI] != (DWORD)D3DEndScene_hook) {
			std::cout << "D3DEndScene hook was changed!" << std::endl;
			*(DWORD*)&IDirect3D9_vtable[ENDSCENE_VTI] = (DWORD)D3DEndScene_hook;
			*(DWORD*)&IDirect3D9_vtable[RESET_VTI] = (DWORD)D3DReset_hook;
		}
	}
}


HRESULT WINAPI D3DCreateDevice_hook(IDirect3D9* Direct3D_Object, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, 
                    DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, 
                    IDirect3DDevice9** ppReturnedDeviceInterface)
{
#ifdef _HKDEBUG_
	MessageBoxA(NULL, "CreateDevice called", "CreateDevice", MB_ICONEXCLAMATION);
#endif
	std::cout << "CreateDevice called" << std::endl;
	// Append the almighty D3DCREATE_MULTITHREADED flag...
	HRESULT result = D3DCreateDevice_orig(Direct3D_Object, Adapter, DeviceType, hFocusWindow, BehaviorFlags | D3DCREATE_MULTITHREADED, pPresentationParameters, ppReturnedDeviceInterface);

	// Now we've intercepted the program's call to CreateDevice and we have the IDirect3DDevice9 that it uses
	// We can get it's vtable and patch in our own detours
	// Reset the CreateDevice hook since it's no longer needed
	// Unprotect the vtable for writing
	DWORD protectFlag;
	if (VirtualProtect(&IDirect3D9_vtable[CREATEDEVICE_VTI], sizeof(DWORD), PAGE_READWRITE, &protectFlag))
	{
		// Store the original CreateDevice pointer and shove our own function into the vtable
		*(DWORD*)&IDirect3D9_vtable[CREATEDEVICE_VTI] = (DWORD)D3DCreateDevice_orig;

		// Reprotect the vtable
		if (!VirtualProtect(&IDirect3D9_vtable[CREATEDEVICE_VTI], sizeof(DWORD), protectFlag, &protectFlag))
		{
			return D3DERR_INVALIDCALL;
		}
	} else {
		return D3DERR_INVALIDCALL;
	}

	if (result == D3D_OK)
	{
		// Load the new vtable
		IDirect3D9_vtable = (DWORD*)*(DWORD*)*ppReturnedDeviceInterface;
#ifdef _HKDEBUG_
		MessageBoxA(NULL, "Loaded IDirect3DDevice9 vtable", "CreateDevice", MB_ICONEXCLAMATION);
#endif
		std::cout << "Loaded IDirect3DDevice9 vtable" << std::endl;
		// Store pointers to the original functions that we want to hook
		*(PDWORD)&D3DEndScene_orig = (DWORD)IDirect3D9_vtable[ENDSCENE_VTI];
		*(PDWORD)&D3DReset_orig = (DWORD)IDirect3D9_vtable[RESET_VTI];

		if (!CreateThread(NULL, 0, VTablePatchThread, NULL, NULL, NULL)) {
			return D3DERR_INVALIDCALL;
		}
	}

	
	std::cout << "Setting hook" << std::endl;
	HWND hWnd = FindWindow(L"MapleStoryClass", NULL);
	if (hWnd != NULL) {
		std::cout << "Setting title" << std::endl;
		SetWindowText(hWnd, L"MapleStory - Mapler.me Enabled");
	}
	else
		std::cout << "NOT FOUND!" << std::endl;


	return result;
}

void CheckDevice(IDirect3DDevice9* device) {
	if ((DWORD*)*(DWORD*)device != IDirect3D9_vtable) {

		IDirect3D9_vtable = (DWORD*)*(DWORD*)device;
	}
}

HRESULT WINAPI D3DEndScene_hook(IDirect3DDevice9* device) {
	CheckDevice(device);
	VariableStorage::GetInstance()->_sDevice = device;
	Hooking::Draw();

	return D3DEndScene_orig(device);
}



HRESULT WINAPI D3DReset_hook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	std::cout << "RESET" << std::endl;
	VariableStorage::GetInstance()->_loadedTextures.clear();

	Hooking::OnLost();
	
	HRESULT res = D3DReset_orig(device, pPresentationParameters);

	if (res == D3D_OK) {
		std::cout << "Reset ok O.o" << std::endl;
		CheckDevice(device);
		Hooking::OnReset();
	}
	else {
		std::cout << "Reset NOT OK!!!" << std::endl;
	}
	
	std::cout << "Reset:" << std::hex << res << std::endl;

	return res;
}





HRESULT WINAPI HookCreateDevice() {
	// Obtain a D3D object
	IDirect3D9* device = Direct3DCreate9(D3D_SDK_VERSION);
	if (!device) {
#ifdef _HKDEBUG_
		MessageBoxA(NULL, "Unable to create device", "CreateDevice", MB_ICONEXCLAMATION);
#endif
		std::cout << "Unable to create device" << std::endl;
		return D3DERR_INVALIDCALL;
	}
	// Now we have an object, store a pointer to its vtable and release the object
	IDirect3D9_vtable = (DWORD*)*(DWORD*)device; // Confusing typecasts
	device->Release();

	// Unprotect the vtable for writing
	DWORD protectFlag;
	if (VirtualProtect(&IDirect3D9_vtable[CREATEDEVICE_VTI], sizeof(DWORD), PAGE_READWRITE, &protectFlag))
	{
		// Store the original CreateDevice pointer and shove our own function into the vtable
		*(DWORD*)&D3DCreateDevice_orig = IDirect3D9_vtable[CREATEDEVICE_VTI];
		*(DWORD*)&IDirect3D9_vtable[CREATEDEVICE_VTI] = (DWORD)D3DCreateDevice_hook;

		// Reprotect the vtable
		if (!VirtualProtect(&IDirect3D9_vtable[CREATEDEVICE_VTI], sizeof(DWORD), protectFlag, &protectFlag))
		{
#ifdef _HKDEBUG_
			MessageBoxA(NULL, "Unable to access vtable", "CreateDevice", MB_ICONEXCLAMATION);
#endif
			std::cout << "Unable to access vtable" << std::endl;
			return D3DERR_INVALIDCALL;
		}
	} else {
#ifdef _HKDEBUG_
		MessageBoxA(NULL, "Unable to access vtable", "CreateDevice", MB_ICONEXCLAMATION);
#endif
		std::cout << "Unable to access vtable" << std::endl;
		return D3DERR_INVALIDCALL;
	}
#ifdef _HKDEBUG_
	MessageBoxA(NULL, "Hooked CreateDevice call", "CreateDevice", MB_ICONEXCLAMATION);
#endif
	std::cout << "Hooked CreateDevice call" << std::endl;

	return D3D_OK;
}

typedef SHORT (WINAPI *tGetAsyncKeyState)(__in  int vKey);
tGetAsyncKeyState GetAsyncKeyStateOriginal;

SHORT WINAPI GetAsyncKeyStateHook(int vKey) {
	SHORT result = GetAsyncKeyStateOriginal(vKey);
	printf("Key: %x, result %d\r\n", vKey, result);

	if (vKey >= 0x41 && vKey <= 0x5A) {
		printf("Character: %c\r\n", 'A' + (vKey - 0x41));
	}
	else if (vKey >= 0x30 && vKey <= 0x39) {
		printf("Number: %c\r\n", '1' + (vKey - 0x30));
	}

	return result;
}

typedef BOOL (WINAPI *tGetCursorPos)(
  _Out_  LPPOINT lpPoint
);

tGetCursorPos GetCursorPosOriginal;

BOOL WINAPI GetCursorPosHook(LPPOINT lpPoint) {
	BOOL result = GetCursorPosOriginal(lpPoint);
	printf("Cursor pos: %d %d\r\n", lpPoint->x, lpPoint->y);
	return result;
}


DWORD WINAPI MainRoutine(LPVOID lpArgs) {
	AllocConsole();
	freopen("CONOUT$", "wb", stdout);
	SetConsoleTitle(TEXT("Mapler.me test console"));

	for (int i = 0; ; i++) {
		if (GetModuleHandle(TEXT("d3d9.dll")) == NULL) {
			if (i == 100) {
				MessageBox(NULL, TEXT("Blargf"), TEXT(""), MB_OK);
				goto remove_hook;
			}
			Sleep(100);
		}
		break;
	}
	
	if (HookCreateDevice() != D3D_OK) {
		goto remove_hook;
	}
	
	GetAsyncKeyStateOriginal = (tGetAsyncKeyState)DetourFunction((PBYTE)GetAsyncKeyState, (PBYTE)GetAsyncKeyStateHook);
	GetCursorPosOriginal = (tGetCursorPos)DetourFunction((PBYTE)GetCursorPos, (PBYTE)GetCursorPosHook);
    

	// Close main window...
	//DestroyWindow(GetTopWindow(NULL));


	while (VariableStorage::GetInstance()->_sRunning) {
		Sleep(1000);
	}

remove_hook:
	MessageBox(NULL, TEXT("Removing hook"), TEXT(""), MB_OK);
	Hooking::TryDejectHook();
	return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	VariableStorage::GetInstance()->_sModule = hModule;
	DisableThreadLibraryCalls(hModule);

	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MainRoutine, NULL, NULL, NULL);
			break;
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

