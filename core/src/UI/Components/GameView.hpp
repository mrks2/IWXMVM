#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
	class GameView : public UIComponent
	{
	public:
		GameView()
		{
			Initialize();
		}

		void Render() final;
		void Release() final;

	private:
		void Initialize() final;

		IDirect3DTexture9* texture = NULL;
		ImVec2 textureSize = ImVec2(0, 0);

		int counter = 0;
	};
}