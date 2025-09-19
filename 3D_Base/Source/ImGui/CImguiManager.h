#pragma once
#include <iostream>
#include <vector>

//ImGui�𓮂����̂Ɏg�p.
#include "ImGui//Library//imgui_impl_dx11.h"
#include "ImGui//Library//imgui_impl_win32.h"

//DirectX11.
#include "Assets//DirectX//DirectX11//CDirectX11.h"

//�p��(�h��)�֎~.
//==================================================
//			ImGui�}�l�[�W���[�N���X.
//==================================================
class CImguiManager final
{
public:
	//Imgui�̃V���O���g����.
	//�ŏ��̈�񂾂�.
	static CImguiManager& GetInstance()
	{
		static CImguiManager instance;
		return instance;
	}

	//������.
	static HRESULT Init(HWND hWnd);

	//���.
	static void Release();

	//�t���[���ݒ�.
	static void SetFrame();

	//�`��.
	static void Render();

	//���̓{�b�N�X�̕\��.
	template<typename T>
	static bool Input(const char* label, T& value, bool isLabel = true, float step = 0.0f, float stepFast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);

	//�X���C�_�[�̕\��.
	template<typename T>
	static void Slider(const char* label, T& value, T valueMin, T valueMax, bool isLabel = true);

	//�R���{�{�b�N�X.
	static std::string Combo(const char* Label, std::string& NowItem, const std::vector<std::string>& List, bool isLabel = false, float space = 100.0f);

	//�`�F�b�N�{�b�N�X�̕\��.
	static bool CheckBox(const char* label, bool& flag, bool isLabel = true);

	//�O���t��\��.
	static void Graph(const char* Label, std::vector<float>& Data, const ImVec2& Size = ImVec2(300.0f, 300.0f));
private:
	CImguiManager();
	~CImguiManager();

	//�����E�R�s�[�̍폜.
	CImguiManager(const CImguiManager& rhs) = delete;
	CImguiManager& operator = (const CImguiManager& rhs) = delete;
};