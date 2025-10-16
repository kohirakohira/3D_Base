#pragma once
#include <iostream>
#include <vector>

//ImGuiを動かすのに使用.
#include "ImGui//Library//imgui_impl_dx11.h"
#include "ImGui//Library//imgui_impl_win32.h"

//DirectX11.
#include "Assets//DirectX//DirectX11//CDirectX11.h"

//継承(派生)禁止.
//==================================================
//			ImGuiマネージャークラス.
//==================================================
class CImguiManager final
{
public:
	//Imguiのシングルトン化.
	//最初の一回だけ.
	static CImguiManager& GetInstance()
	{
		static CImguiManager instance;
		return instance;
	}

	//初期化.
	static HRESULT Init(HWND hWnd);

	//解放.
	static void Release();

	//フレーム設定.
	static void SetFrame();

	//描画.
	static void Render();

	//入力ボックスの表示.
	template<typename T>
	static bool Input(const char* label, T& value, bool isLabel = true, float step = 0.0f, float stepFast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);

	//スライダーの表示.
	template<typename T>
	static void Slider(const char* label, T& value, T valueMin, T valueMax, bool isLabel = true);

	//コンボボックス.
	static std::string Combo(const char* Label, std::string& NowItem, const std::vector<std::string>& List, bool isLabel = false, float space = 100.0f);

	//チェックボックスの表示.
	static bool CheckBox(const char* label, bool& flag, bool isLabel = true);

	//グラフを表示.
	static void Graph(const char* Label, std::vector<float>& Data, const ImVec2& Size = ImVec2(300.0f, 300.0f));
private:
	CImguiManager();
	~CImguiManager();

	//生成・コピーの削除.
	CImguiManager(const CImguiManager& rhs) = delete;
	CImguiManager& operator = (const CImguiManager& rhs) = delete;
};