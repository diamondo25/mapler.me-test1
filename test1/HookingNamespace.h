#pragma once

#include <map>

namespace Hooking {
	void Init();
	void TryDejectHook();

	void Draw();

	void OnLost();
	void OnReset();

	std::string CreateScreenshot();

	extern ID3DXFont *m_fontArial;
	extern bool bInitialized;
	extern CDrawableRectangle *m_mainRect;
	extern CTexturedRectangle *m_logo;
};