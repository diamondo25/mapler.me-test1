#pragma once
const DWORD D3D_FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

class CDrawableRectangle {
public:

	struct Vertex {
		FLOAT x, y, z, rhw;
		DWORD color;
	};

	CDrawableRectangle(INT x, INT y, INT w, INT h, DWORD Color) :
	_x(x),
	_y(y),
	_width(w),
	_height(h),
	_color(Color) {
		Init();
	}

	~CDrawableRectangle(void)
	{
		_vertexBuffer->Release();
	}

	void Init() {
		Vertex V[4] =
		{
			{(FLOAT)_x,				(FLOAT)(_y + _height),	3.0f, 0.0f, _color},
			{(FLOAT)_x,				(FLOAT)_y,				3.0f, 0.0f, _color},
			{(FLOAT)(_x + _width),	(FLOAT)(_y + _height),	3.0f, 0.0f, _color},
			{(FLOAT)(_x + _width),	(FLOAT)_y,				3.0f, 0.0f, _color}
		};

		HRESULT res = VariableStorage::GetInstance()->_sDevice->CreateVertexBuffer(
			4 * sizeof(Vertex),  
			D3DUSAGE_WRITEONLY, 
			D3D_FVF, 
			D3DPOOL_MANAGED, 
			&_vertexBuffer, 
			NULL);
		if (res != D3D_OK) std::cout << "D3D 4: " << res << std::endl;
		
		Vertex * v;
		_vertexBuffer->Lock(0, 0, (void**)&v, 0);

		v[0] = V[0];
		v[1] = V[1];
		v[2] = V[2];
		v[3] = V[3];
		
		_vertexBuffer->Unlock();
	}


	void Draw() {
		HRESULT res = VariableStorage::GetInstance()->_sDevice->SetFVF(D3D_FVF);
		if (res != D3D_OK) std::cout << "D3D 1: " << res << std::endl;

		res = VariableStorage::GetInstance()->_sDevice->SetStreamSource(0, _vertexBuffer, 0, sizeof(Vertex));
		if (res != D3D_OK) std::cout << "D3D 2: " << res << std::endl;

		VariableStorage::GetInstance()->_sDevice->SetTexture(0, NULL);

		res = VariableStorage::GetInstance()->_sDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		if (res != D3D_OK) std::cout << "D3D 3: " << res << std::endl;
	}
private:
	IDirect3DVertexBuffer9 * _vertexBuffer;
	Vertex *_vertexData;

	int _x, _y, _width, _height;
	DWORD _color;
};

