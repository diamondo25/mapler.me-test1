#pragma once

#include <vector>

class VariableStorage {
public:
	static VariableStorage * GetInstance() {
		if (_instance == nullptr)
			_instance = new VariableStorage();
		return _instance;
	}

	void LoadTexture(const std::wstring &pName) {
		std::wcout << "Loading texture " << pName << std::endl;
		if (_loadedTextures.find(pName) == _loadedTextures.end()) {
			IDirect3DTexture9 * texture;
			HRESULT res = D3DXCreateTextureFromFile(_sDevice, pName.c_str(), &texture);

			_loadedTextures[pName] = texture;
			std::cout << "Not found, loaded with result " << res << "; ID " << texture << std::endl;
		}
	}

	void LoadTextureFromResource(const std::wstring &pName, LPWSTR pIdentifier) {
		std::wcout << "Loading texture " << pName << std::endl;
		if (_loadedTextures.find(pName) == _loadedTextures.end()) {
			IDirect3DTexture9 * texture;
			//HRESULT res = D3DXCreateTextureFromResource(_sDevice, _sModule, pIdentifier, &texture);
			HRESULT res = D3DXCreateTextureFromResourceEx(_sDevice, _sModule, pIdentifier, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, 
				D3DPOOL_MANAGED, 
				D3DX_FILTER_TRIANGLE, D3DX_FILTER_NONE, 
				0, 
				0,
				0,
				&texture);

			_loadedTextures[pName] = texture;
			std::cout << "Not found, loaded with result " << std::hex << res << "; ID " << texture << std::endl;
		}
	}
	
	
	HMODULE _sModule;
	bool _sRunning;
	IDirect3DDevice9 * _sDevice;
	std::map<std::wstring, IDirect3DTexture9 *> _loadedTextures;
private:
	static VariableStorage * _instance;

	VariableStorage() {
		_sModule = 0;
		_sRunning = true;
		_sDevice = nullptr;
	}
};