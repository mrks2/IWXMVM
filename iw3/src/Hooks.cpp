#include "StdInclude.hpp"
#include "Hooks.hpp"

#include "Structures.hpp"
#include "Events.hpp"
#include "Utilities/HookManager.hpp"
#include "IW3Interface.hpp"
#include "Mod.hpp"

namespace IWXMVM::IW3::Hooks
{
	// TODO: Turn this into a "EventCallbacks" "patch"

	typedef HRESULT(__stdcall* EndScene_t)(IDirect3DDevice9* pDevice);
	EndScene_t EndScene;

	HRESULT __stdcall D3D_EndScene_Hook(IDirect3DDevice9* pDevice)
	{
		Events::Invoke(EventType::OnFrame);

		return EndScene(pDevice);
	}

	typedef HRESULT(__stdcall* Reset_t)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	Reset_t	Reset;

	HRESULT __stdcall D3D_Reset_Hook(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		HRESULT hr = Reset(pDevice, pPresentationParameters);
		ImGui_ImplDX9_CreateDeviceObjects();

		return hr;
	}

	int realtime = 0;

	// run every frame
	void CL_CGameRendering_Internal()
	{
		if (Mod::GetGameInterface()->IsDemoPlaybackPaused()) 
		{
			if (realtime == 0) 
			{
				realtime = *(int*)0x956E98; // cls->realtime
			}

			*(int*)0x956E98 = realtime;
		}
		else 
		{
			realtime = 0;
		}
	}

	typedef void(*CL_CGameRendering_t)();
	CL_CGameRendering_t CL_CGameRendering;
	void __declspec(naked) CL_CGameRendering_Hook()
	{
		__asm pushad

		CL_CGameRendering_Internal();

		__asm
		{
			popad
			jmp CL_CGameRendering
		}
	}

	void CL_PlayDemo_Internal(uintptr_t** address, char* cmd)
	{
		if (address != 0) 
		{
			reinterpret_cast<uintptr_t(*)()>(*address)();

			if (!strcmp(cmd, "demo")) 
			{
				Events::Invoke(EventType::OnDemoLoad);
			}
		}
	}

	uintptr_t* ptrCL_PlayDemo_f = nullptr;
	Structures::cmd_function_t** sv_cmd_functions = (Structures::cmd_function_t**)0x14099DC;

	void Cmd_ModifyServerCommand(const char* cmd_name, void* function)
	{
		for (auto cmd = *sv_cmd_functions; cmd; cmd = cmd->next) 
		{
			if (!strcmp(cmd_name, cmd->name) && function != nullptr) 
			{
				if (function != nullptr) 
				{
					ptrCL_PlayDemo_f = (uintptr_t*)cmd->function;
					cmd->function = function;

					break;
				}
			}
		}
	}

	void CL_PlayDemo_Hook()
	{
		for (auto cmd = *sv_cmd_functions; cmd; cmd = cmd->next) 
		{
			if (!strcmp(cmd->name, "demo") && ptrCL_PlayDemo_f != nullptr) 
			{
				reinterpret_cast<uintptr_t(*)()>(ptrCL_PlayDemo_f)();

				Events::Invoke(EventType::OnDemoLoad);
				break;
			}
		}
	}

	void Install(IDirect3DDevice9* device)
	{
		void** vTable = *reinterpret_cast<void***>(device);

		EndScene = (EndScene_t)HookManager::CreateHook((std::uintptr_t)vTable[42], (std::uintptr_t)&D3D_EndScene_Hook, 7, true);
		Reset = (Reset_t)HookManager::CreateHook((std::uintptr_t)vTable[16], (std::uintptr_t)&D3D_Reset_Hook, 5, true);

		Cmd_ModifyServerCommand("demo", CL_PlayDemo_Hook);
		CL_CGameRendering = (CL_CGameRendering_t)HookManager::CreateHook(0x474DA0, (std::uintptr_t)&CL_CGameRendering_Hook, 7, true);
	}
}