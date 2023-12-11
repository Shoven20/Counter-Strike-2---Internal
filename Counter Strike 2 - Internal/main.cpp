#include "includes.h"
#include <thread>
#include <string>
#include "hile.h"

bool showmenu = true;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;
bool init = false;

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

inline void Draw_Line(int X, int Y, int W, int H, ImColor color, int thickness = 1.f, bool stroke = false) {
	if (stroke)
	{
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X + 1, Y + 1), ImVec2(W + 1, H + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), thickness);
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X - 1, Y - 1), ImVec2(W - 1, H - 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), thickness);
	}
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(X, Y), ImVec2(W, H), ImGui::ColorConvertFloat4ToU32(color), thickness);
}
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init) {
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)& pDevice))) {
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			RECT game; GetWindowRect(window, &game);
			screensize = { (float)game.right,(float)game.bottom };
			init = true;
		} else return oPresent(pSwapChain, SyncInterval, Flags);
	}

	HileVoid();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	std::string countString = "Number of Players Alive: " + std::to_string(player_prefabs.size() + 1);
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(15, 15), ImGui::ColorConvertFloat4ToU32(ImColor(255, 255, 255)), countString.c_str());
	for (int i = 0; i < player_prefabs.size(); i++)
	{	
		playerstruct entry = player_prefabs[i];
		if (Skeleton) {
			Vector3 Bone1; if (!w2s(entry.BoneArray[6].Pos, Bone1)) continue;
			Vector3 Bone2; if (!w2s(entry.BoneArray[7].Pos, Bone2)) continue;
			Vector3 Bone3; if (!w2s(entry.BoneArray[13].Pos, Bone3)) continue;
			Vector3 Bone5; if (!w2s(entry.BoneArray[14].Pos, Bone5)) continue;
			Vector3 Bone6; if (!w2s(entry.BoneArray[20].Pos, Bone6)) continue;
			Vector3 Bone4; if (!w2s(entry.BoneArray[8].Pos, Bone4)) continue;
			Vector3 Bone7; if (!w2s(entry.BoneArray[9].Pos, Bone7)) continue;
			Vector3 Bone8; if (!w2s(entry.BoneArray[21].Pos, Bone8)) continue;
			Vector3 Bone9; if (!w2s(entry.BoneArray[19].Pos, Bone9)) continue;
			Vector3 Bone10; if (!w2s(entry.BoneArray[0].Pos, Bone10)) continue;
			Vector3 Bone11; if (!w2s(entry.BoneArray[25].Pos, Bone11)) continue;
			Vector3 Bone12; if (!w2s(entry.BoneArray[22].Pos, Bone12)) continue;
			Vector3 Bone13; if (!w2s(entry.BoneArray[26].Pos, Bone13)) continue;
			Vector3 Bone14; if (!w2s(entry.BoneArray[27].Pos, Bone14)) continue;
			Vector3 Bone15; if (!w2s(entry.BoneArray[23].Pos, Bone15)) continue;
			Vector3 Bone16; if (!w2s(entry.BoneArray[24].Pos, Bone16)) continue;
			Draw_Line(Bone1.x, Bone1.y, Bone2.x, Bone2.y, entry.EspColor, 1.f, true);
			Draw_Line(Bone2.x, Bone2.y, Bone3.x, Bone3.y, entry.EspColor, 1.f, true);
			Draw_Line(Bone2.x, Bone2.y, Bone4.x, Bone4.y, entry.EspColor, 1.f, true);
			Draw_Line(Bone3.x, Bone3.y, Bone5.x, Bone5.y, entry.EspColor, 1.f, true);
			Draw_Line(Bone5.x, Bone5.y, Bone6.x, Bone6.y, entry.EspColor, 1.f, true);
			Draw_Line(Bone4.x, Bone4.y, Bone7.x, Bone7.y, entry.EspColor, 1.f, true);
			Draw_Line(Bone7.x, Bone7.y, Bone8.x, Bone8.y, entry.EspColor, 1.f, true);
			Draw_Line(Bone2.x, Bone2.y, Bone9.x, Bone9.y, entry.EspColor, 1.f, true);
			Draw_Line(Bone9.x, Bone9.y, Bone10.x, Bone10.y, entry.EspColor, 1.f, true);
			Draw_Line(Bone10.x, Bone10.y, Bone10.x, Bone10.y, entry.EspColor, 1.f, true);
			Draw_Line(Bone10.x, Bone10.y, Bone11.x, Bone11.y, entry.EspColor, 1.f, true);
			Draw_Line(Bone10.x, Bone10.y, Bone12.x, Bone12.y, entry.EspColor, 1.f, true);
			Draw_Line(Bone11.x, Bone11.y, Bone13.x, Bone13.y, entry.EspColor, 1.f, true);
			Draw_Line(Bone13.x, Bone13.y, Bone14.x, Bone14.y, entry.EspColor, 1.f, true);
			Draw_Line(Bone12.x, Bone12.y, Bone15.x, Bone15.y, entry.EspColor, 1.f, true);
			Draw_Line(Bone15.x, Bone15.y, Bone16.x, Bone16.y, entry.EspColor, 1.f, true);
		}
	}

	if (GetAsyncKeyState(VK_INSERT) & 1) showmenu = !showmenu;
	if (showmenu)
	{
		ImGui::SetNextWindowSize(ImVec2(160, 120));
		ImGui::Begin("SHOVEN - CS2 INTERNAL", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
		ImGui::Checkbox("Skeleton", &Skeleton);
		ImGui::ColorEdit4("Enemy Esp Color", (float*)&enemyespcolor, ImGuiColorEditFlags_NoDragDrop | ImGuiWindowFlags_NoScrollbar | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
		ImGui::ColorEdit4("Team Esp Color", (float*)&teamespcolor, ImGuiColorEditFlags_NoDragDrop | ImGuiWindowFlags_NoScrollbar | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
		ImGui::End();
	}
	ImGui::Render();
	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}
 
DWORD WINAPI MainThread(LPVOID lpReserved)
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)& oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);
	return TRUE;
}

bool init_hook = false;
BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		shovenmemory.InitCheat();
		DisableThreadLibraryCalls(hMod);
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}