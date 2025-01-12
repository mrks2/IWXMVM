#pragma once
#include "UI/UIComponent.hpp"

namespace IWXMVM::UI
{
	class Background : public UIComponent
	{
	public:
		void Render() final;
		void Release() final;

	private:
		void Initialize() final;
	};
}