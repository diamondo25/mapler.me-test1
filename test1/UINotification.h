#pragma once

class CUINotification {
public:
	CUINotification() {
		_texTop = _texMid = _texBot = nullptr;
		Init();
	}

	void Init() {
		auto vs = VariableStorage::GetInstance();

		if (_texTop != nullptr) {
			std::cout << "Removing..." << std::endl;
			delete _texTop;
			delete _texMid;
			delete _texBot;
			delete _logo;
		}
		
		_texTop = new CTexturedRectangle(0, 0, NOTIFICATION_WIDTH, 40, 0, 0, NOTIFICATION_WIDTH, 40, L"notification_top", IDB_NOTIFICATION_TOP);
		_texMid = new CTexturedRectangle(0, 0, NOTIFICATION_WIDTH, 8,  0, 0, NOTIFICATION_WIDTH, 8,  L"notification_mid", IDB_NOTIFICATION_MIDDLE);
		_texBot = new CTexturedRectangle(0, 0, NOTIFICATION_WIDTH, 10, 0, 0, NOTIFICATION_WIDTH, 10, L"notification_bot", IDB_NOTIFICATION_BOTTOM);
		
		_logo = new CTexturedRectangle(0, 0, 32, 32, 0, 0, 32, 32, L"logo", IDB_LOGO);


		if (_textFont == nullptr) {
			std::cout << "Creating Font..." << std::endl;
			D3DXCreateFont(VariableStorage::GetInstance()->_sDevice, 14, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &_textFont);
		}
	}

	static const int NOTIFICATION_WIDTH = 261;

	void Draw(const std::wstring &pText) {

		D3DVIEWPORT9 viewport;
		VariableStorage::GetInstance()->_sDevice->GetViewport(&viewport);

		
		int pX = 0;
		int pY = 0;

		int modi = DT_WORDBREAK | DT_TOP | DT_NOCLIP;

		RECT rct;
		rct.left = 10;
		rct.right = NOTIFICATION_WIDTH - 20 - _logo->GetWidth() - 5;
		rct.top = 0;
		rct.bottom = 0;

		int height = _textFont->DrawTextW(NULL, pText.c_str(), -1, &rct, 
			modi | DT_CALCRECT
			, _textColor);

		// We know the size now

		pX = viewport.Width - NOTIFICATION_WIDTH;

		pY = viewport.Height;
		pY -= _texTop->GetHeight();
		pY -= _texBot->GetHeight();
		pY -= height - 12;

		rct.top = pY + 10;
		rct.bottom += rct.top - 10;
		rct.left = pX + 10;
		rct.right += rct.left;

		_texTop->SetPosition(pX, pY);
		_texMid->SetPosition(pX, pY + _texTop->GetHeight());

		_texMid->SetSize(_texMid->GetWidth(), height - 12);
		_texBot->SetPosition(pX, pY + _texTop->GetHeight() + _texMid->GetHeight());
		
		_texTop->Draw();
		_texMid->Draw();
		_texBot->Draw();

		_textFont->AddRef();

		_textFont->DrawTextW(NULL, pText.c_str(), -1, &rct, modi, _textColor);

		_textFont->Release();

		_logo->SetPosition(pX + NOTIFICATION_WIDTH - 10 - _logo->GetWidth(), pY + 10); // Place at the right
		_logo->Draw();
	}

	void OnLost() {
		_textFont->OnLostDevice();
	}

	void OnReset() {
		_textFont->OnResetDevice();

		Init();
	}
private:
	CTexturedRectangle *_texTop, *_texMid, *_texBot;
	CTexturedRectangle *_logo;

	static ID3DXFont *_textFont;
	static const D3DCOLOR _textColor =  D3DCOLOR_ARGB(255, 255, 255, 255);
};

ID3DXFont *CUINotification::_textFont = nullptr;