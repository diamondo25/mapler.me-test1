#pragma once

const DWORD D3D_FVF_TEXTURED = D3DFVF_XYZRHW | D3DFVF_TEX1;
#define FULL_UI_WIDTH 512
#define FULL_UI_HEIGHT 256

class CTexturedRectangle {
public:

	struct Vertex {
		float x, y, z, rhw;
		float v, w;
	};

	CTexturedRectangle(int x, int y, int w, int h, int tX, int tY, int tW, int tH, const std::wstring &pName, int pIdentifier = -1) :
	_x(x),
	_y(y),
	_width(w),
	_height(h),
	_textureX(tX),
	_textureY(tY),
	_textureW(tW),
	_textureH(tH) {

		SetTexture(pName, pIdentifier);
	}

	~CTexturedRectangle(void) {
	}
	
	// 100,40 = 40 / 100 = 0.4
	//#define TEX_POINT(wh, xy) (float)((float)(xy / (float)wh)
	float TEX_POINT(int wh, int xy) {
		return (float)((float)xy / (float)wh);
	}

	void Draw() {
		VariableStorage * vs = VariableStorage::GetInstance();

		IDirect3DTexture9 * texture = vs->_loadedTextures[_textureName];

		texture->AddRef();

		HRESULT res = vs->_sDevice->SetFVF(D3D_FVF_TEXTURED);
		if (res != D3D_OK) std::cout << "D3D1 1: " << res << std::endl;

		vs->_sDevice->SetTexture(0, texture);
		vs->_sDevice->SetTexture(1, NULL);


		Vertex V[4] =
		{
			{(FLOAT)_x,				(FLOAT)(_y + _height),		0.2f, 0.0f, TEX_POINT(_textureW, _textureX),				TEX_POINT(_textureH, _textureY + _height)},
			{(FLOAT)_x,				(FLOAT)_y,					0.2f, 0.0f, TEX_POINT(_textureW, _textureX),				TEX_POINT(_textureH, _textureY)},
			{(FLOAT)(_x + _width),	(FLOAT)(_y + _height),		0.2f, 0.0f, TEX_POINT(_textureW, _textureX + _width),	TEX_POINT(_textureH, _textureY + _height)},
			{(FLOAT)(_x + _width),	(FLOAT)_y,					0.2f, 0.0f, TEX_POINT(_textureW, _textureX + _width),	TEX_POINT(_textureH, _textureY)}
		};

		res = vs->_sDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &V, sizeof(Vertex));

		if (res != D3D_OK) std::cout << "D3D1 3: " << res << std::endl;

		texture->Release();
	}

	void SetPosition(int pX, int pY) {
		_x = pX;
		_y = pY;
	}

	int GetX() {
		return _x;
	}

	int GetY() {
		return _y;
	}

	void SetSize(int pWidth, int pHeight) {
		_width = pWidth;
		_height = pHeight;
	}

	int GetWidth() {
		return _width;
	}

	int GetHeight() {
		return _height;
	}

	int SetTextureXY(int x, int y) {
		_textureX = x;
		_textureY = y;
	}

private:
	std::wstring _textureName;

	int _x, _y, _width, _height;
	int _textureX, _textureY, _textureW, _textureH;
	DWORD _color;

	void SetTexture(const std::wstring &pName, int pIdentifier = -1) {
		_textureName = pName;
		if (pIdentifier == -1)
			VariableStorage::GetInstance()->LoadTexture(_textureName);
		else
			VariableStorage::GetInstance()->LoadTextureFromResource(_textureName, MAKEINTRESOURCE(pIdentifier));
	}
};

