#include "HookingNamespace.h"
#include "UINotification.h"
#include <sstream>
#include <time.h>

ID3DXFont * Hooking::m_fontArial = nullptr;
bool Hooking::bInitialized = false;
CDrawableRectangle * Hooking::m_mainRect = nullptr;
CTexturedRectangle * Hooking::m_logo = nullptr;
int playtime = 0;

CUINotification *notification = nullptr;

void Hooking::Init() {
	D3DXCreateFont(VariableStorage::GetInstance()->_sDevice, 20, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Calibri"), &m_fontArial);
	
	if (m_mainRect != nullptr)
		delete m_mainRect;
	m_mainRect = new CDrawableRectangle(2, 2, 300, 34, D3DCOLOR_ARGB(200, 50, 120, 12));
	
	if (m_logo != nullptr) {
		delete m_logo;
	}
	m_logo = new CTexturedRectangle(3, 3, 32, 32, 0, 0, 32, 32, L"logo", IDB_LOGO);
	
	if (notification != nullptr)
		delete notification;
	notification = new CUINotification();

	// _characterInfo = new CTexturedRectangle(100, 100, 301, 158, 0, 82, FULL_UI_WIDTH, FULL_UI_HEIGHT, L"full_ui", IDB_FULL_UI);
}

void Hooking::TryDejectHook() {
	MessageBox(NULL, TEXT("EXITING!!!"), TEXT("GOI"), MB_OK);
	FreeLibraryAndExitThread(VariableStorage::GetInstance()->_sModule, 0);
}

void Hooking::Draw() {
	if (!bInitialized) {
		if (playtime == 0) {
			playtime = time(NULL);
		}
		std::cout << "Initializing stuff.." << std::endl;
		OutputDebugStringA("Initializing 3D stuff (Mapler.me)");
		Init();
		bInitialized = true;
	}

	VariableStorage *vs = VariableStorage::GetInstance();
	
	DWORD curFVF;
	vs->_sDevice->GetFVF(&curFVF);


	vs->_sDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	vs->_sDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	vs->_sDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	vs->_sDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
	vs->_sDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	vs->_sDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

	m_mainRect->Draw();
	m_logo->Draw();

	D3DCOLOR fontColor = D3DCOLOR_ARGB(255, 255, 255, 255);   

	// Create a rectangle to indicate where on the screen it should be drawn
	RECT rct;
	rct.left = 40;
	rct.right = 300;
	rct.top = 13;
	rct.bottom = rct.top + 20;

	int secondsRan = time(NULL) - playtime;
	int timeSecs = floor(secondsRan % 60);
	int timeMins = floor((secondsRan / 60) % 60);
	int timeHours = floor(secondsRan / 60 / 60);

	
	char buff[100];
	sprintf(buff, "Mapler.me - Playtime %02d:%02d:%02d", timeHours, timeMins, timeSecs);

 
	// Draw some text
	m_fontArial->DrawTextA(NULL, buff, -1, &rct, 0, fontColor);

	
	notification->Draw(L"Mapler.me service initialized...");

	// Reset all stuffs
	vs->_sDevice->SetFVF(curFVF);
}

void Hooking::OnLost() {
	std::cout << "lost1" << std::endl;
	m_fontArial->OnLostDevice();
	notification->OnLost();
}

void Hooking::OnReset() {
	std::cout << "reset1" << std::endl;
	m_fontArial->OnResetDevice();
	notification->OnReset();
	bInitialized = false;
}

std::string Hooking::CreateScreenshot() {
	std::wstringstream ss;
	ss << "screenshot-";
	ss << time(NULL);
	ss << ".png";

	IDirect3DDevice9 *device = VariableStorage::GetInstance()->_sDevice;

	IDirect3DSurface9 *surface;
	D3DVIEWPORT9 viewport;
	device->GetViewport(&viewport);

	HRESULT result = device->CreateOffscreenPlainSurface(viewport.Width, viewport.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &surface, NULL);
	//CHECK_DX_ERROR;

	result = device->GetFrontBufferData(0, surface);
	//CHECK_DX_ERROR;

	result = D3DXSaveSurfaceToFile(ss.str().c_str(), D3DXIFF_BMP, surface, NULL, NULL);
	//CHECK_DX_ERROR;
	return "uwotm8";
}