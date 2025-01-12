#include "StdInclude.hpp"
#include "ControlBar.hpp"

#include "Mod.hpp"
#include "CustomImGuiControls.hpp"
#include "UI/UIImage.hpp"
#include "UI/UIManager.hpp"
#include "Utilities/PathUtils.hpp"

namespace IWXMVM::UI
{
	float playbackSpeed;

	std::optional<Types::Dvar> timescale;

	void ControlBar::Initialize()
	{
		playbackSpeed = 1.0f;
	}

	INCBIN_EXTERN(IMG_PLAY_BUTTON);
	INCBIN_EXTERN(IMG_PAUSE_BUTTON);

	void ControlBar::Render()
	{
		if (Mod::GetGameInterface()->GetGameState() != Types::GameState::InDemo)
			return;

		if (!timescale.has_value())
		{
			timescale = Mod::GetGameInterface()->GetDvar("timescale");
			return;
		}

		SetPosition(
			0,
			UIManager::Get().GetUIComponent(UI::Component::MenuBar)->GetSize().y + UIManager::Get().GetUIComponent(UI::Component::GameView)->GetSize().y
		);
		SetSize(
			ImGui::GetIO().DisplaySize.x,
			ImGui::GetIO().DisplaySize.y - GetPosition().y
		);

		ImGui::SetNextWindowPos(GetPosition());
		ImGui::SetNextWindowSize(GetSize());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
		if (ImGui::Begin("Playback Controls", nullptr, flags))
		{
			constexpr auto PADDING = 20;
			ImGui::SetCursorPosX(PADDING);

			auto pauseButtonSize = ImVec2(ImGui::GetFontSize() * 1.4f, ImGui::GetFontSize() * 1.4f);

			auto image = UIImage::FromResource(Mod::GetGameInterface()->IsDemoPlaybackPaused() ? IMG_PLAY_BUTTON_data : IMG_PAUSE_BUTTON_data,
				Mod::GetGameInterface()->IsDemoPlaybackPaused() ? IMG_PLAY_BUTTON_size : IMG_PAUSE_BUTTON_size);
			if (ImGui::ImageButton(image.GetTextureID(), pauseButtonSize, ImVec2(0, 0), ImVec2(1, 1), 1))
			{
				Mod::GetGameInterface()->ToggleDemoPlaybackState();
			}

			const auto playbackSpeedSliderWidth = GetSize().x / 8;

			ImGui::SetNextItemWidth(playbackSpeedSliderWidth);
			ImGui::SameLine();
			ImGui::TimescaleSlider("##", &timescale.value().value->floating_point, 0.001f, 1000.0f, "%.3f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);

			auto demoInfo = Mod::GetGameInterface()->GetDemoInfo();
			auto progressBarX = PADDING + pauseButtonSize.x + playbackSpeedSliderWidth + PADDING * 3;
			auto progressBarWidth = GetSize().x - progressBarX - GetSize().x * 0.05f - PADDING;
			ImGui::SameLine(progressBarX);
			ImGui::DemoProgressBar(&demoInfo.currentTick, demoInfo.endTick, ImVec2(progressBarWidth, GetSize().y / 3.4f), std::format("{0}", demoInfo.currentTick).c_str());

			ImGui::End();
		}
	}

	void ControlBar::Release()
	{
		UIImage::ReleaseResource(IMG_PLAY_BUTTON_data);
		UIImage::ReleaseResource(IMG_PAUSE_BUTTON_data);
	}
}
