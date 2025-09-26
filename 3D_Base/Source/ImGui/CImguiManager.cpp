#include "CImguiManager.h"
#include <utility>

constexpr char FONT_FILE_PATH[]		= "Data//ImGui//Font//NotoSansJP-SemiBold.ttf";
constexpr float FONT_SIZE			= 18.0f;
constexpr float SAMALINE_OFFSET		= 100.0f;//ImGui::SameLine�̃I�t�Z�b�g�l.

CImguiManager::CImguiManager()
{
}

CImguiManager::~CImguiManager()
{
}

//������.
HRESULT CImguiManager::Init(HWND hWnd)
{
	//�R���e�L�X�g(ImGui�̐��E)�̍쐬.
	ImGui::CreateContext();

	//imgui.h�̒��ɂ���\���́�ImGui �̋�����ݒ�A���͏�� ...etc.
	//�Q�[���̃E�B���h�E�� ImGui ��\���ł���悤�ɂ��鏀����.
	ImGuiIO io = ImGui::GetIO();
	//�t���[�����ԁA�E�B���h�E�T�C�Y�A�t���O�Ȃǂ̐ݒ�.
	//�u|=�v: �r�b�g���Ƃ�OR���Z�q���킩��Ȃ����͒��ׂ�.
	io.ConfigFlags		|= ImGuiConfigFlags_DockingEnable;		//�E�B���h�E���h���b�O���č��́E��������̂�L���ɂ���.
	io.ConfigFlags		|= ImGuiConfigFlags_ViewportsEnable;	//ImGui �̃E�B���h�E���Q�[���E�B���h�E�O�ɏo����.
	io.DeltaTime		 = 1.0f / 60.0f;						//1�t���[���̎���.
	io.DisplaySize.x	 = WND_WF;								//���̉�ʃT�C�Y.
	io.DisplaySize.x	 = WND_HF;								//�c�̉�ʃT�C�Y.

	//�t�H���g�̓ǂݍ��݁��t�H���g�̐؂�ւ��A�������̌v�Z�A�e�L�X�g�`�揈��...etc.
	ImFont* font = io.Fonts->AddFontFromFileTTF(
		FONT_FILE_PATH,							//�t�H���g�t�@�C���̃p�X.
		FONT_SIZE,								//�t�H���g�T�C�Y(�s�N�Z���P��).
		NULL,									//�t�H���g�̏ڍאݒ聦����͓��Ɏw�肵�Ȃ�.
		io.Fonts->GetGlyphRangesJapanese());	//�g�������͈̔�(�O���t�͈�)�w�聦�w�肵�Ȃ���ASCII���炢��������Ȃ��B����͓��{��(���ȁA�J�i�A�����ꕔ)�ł����v.

	//�����Ŋ�{�̐F��t�H���g�T�C�Y�Ȃǂ��u�ЂȌ`�v�Ƃ��Č��߂遫.
	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsDefalt();
	ImGui::StyleColorsLight();

	//�E�B���h�E�̊ۂ݂�F���J�X�^�}�C�Y�ł���p�ɂ���.
	ImGuiStyle style = ImGui::GetStyle();
	//UI�E�B���h�E�����C����ʂ���؂藣���ĕʃE�B���h�E�Ɉړ��ł���@�\��ON�ɂȂ��Ă���?
	//�u&�v: �r�b�g���Ƃ�AND���Z�q���킩��Ȃ����͒��ׂ�.
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 5.0f;				//�E�B���h�E�̊p���ۂ߂��A�t���b�g�Ȏl�p�ɂ���.
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;	//�w�i�F�̃A���t�@(�����x)��100%(�s����)�ɂ���.
	}

	//Win32 �v���b�g�t�H�[���p�o�b�N�G���h��������.
	//���͏���(�}�E�X�A�L�[�{�[�h�AIME)��ImGui�Ŏg�p�ł���悤�ɂ���.
	if (ImGui_ImplWin32_Init(hWnd) == false)
	{
		return E_FAIL;
	}
	//DirectX11 �����_���[�o�b�N�G���h ��������.
	//���O�̕`�施�߂�DirectX11�ɓ�������悤�ɂ���.
	if (ImGui_ImplDX11_Init(CDirectX11::GetInstance().GetDevice(), CDirectX11::GetInstance().GetContext()) == false)
	{
		return E_FAIL;
	}

	return S_OK;
}
//���.
void CImguiManager::Release()
{
	ImGui_ImplDX11_Shutdown();		//DirectX11�p�o�b�N�G���h�̏I������.
	ImGui_ImplWin32_Shutdown();		//Win32�p�o�b�N�G���h�̏I������.
	ImGui::DestroyContext();		//ImGui�S�̂̃R���e�L�X�g(���E)��j��.
}

//�t���[���ݒ�.
void CImguiManager::SetFrame()
{
	ImGui_ImplDX11_NewFrame();		//DirectX11���̐V�t���[������.
	ImGui_ImplWin32_NewFrame();		//Win32���̐V�t���[������.
	ImGui::NewFrame();				//ImGui�{�̂̐V�t���[���J�n.
}

//�`��.
void CImguiManager::Render()
{
	//ImGui::GetIO()�FImGui�̓��́E�o�͐ݒ���܂Ƃ߂��uIO(Input/Output)�v�I�u�W�F�N�g��Ԃ��֐�.
	//(void)io;�F�u���̕ϐ��͂킴�Ǝg���Ă܂���I�v�ƃR���p�C���ɓ`����.
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//�`��.
	ImGui::Render();
	//DirectX11�ɕ`��f�[�^��n���A���ۂ�GPU�ŕ`�悳����.
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	//�����E�B���h�E���T�|�[�g���Ă��鎞�ɒǉ�����.
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();			//�V�����J�����E�B���h�E�̏����X�V.
		ImGui::RenderPlatformWindowsDefault();	//���C���E�B���h�E�ȊO��ImGui�E�B���h�E���`��.
	}
}

//���̓{�b�N�X�̕\��.
//�^���ォ��w��ł���֐���.
template<typename T>
inline bool CImguiManager::Input(const char* label, T& value, bool isLabel, float step, float stepFast, const char* format, ImGuiInputTextFlags flags)
{
	//���x��(������)�̕�������쐬��"##"�FImGui��ID�Ǘ��p.
	const std::string newlabel = "##" + std::string(label);

	//���x��(������)�ݒ肪�L���Ȃ烉�x����\��.
	if (isLabel == true)
	{
		ImGui::Text(label);					//���x��(������)�����\��.
		ImGui::SameLine(SAMALINE_OFFSET);	//���x���̉��ɓ��͗��\��.
	}

	//if constexpr�F�R���p�C�����Ɍ^�𔻒肵�āA�K�v�ȕ��򂾂����c��.
	// int�^�ŕ\��.
	if constexpr (std::is_same_v<T, int>) 
	{
		return ImGui::InputInt(newlabel.c_str(), &value, static_cast<int>(step), static_cast<int>(stepFast), flags);
	}
	// float�^�ŕ\��.
	else if constexpr (std::is_same_v<T, float>) 
	{
		return ImGui::InputFloat(newlabel.c_str(), &value, step, stepFast, format, flags);
	}
	// double�^�ŕ\��.
	else if constexpr (std::is_same_v<T, double>) 
	{
		return ImGui::InputDouble(newlabel.c_str(), &value, static_cast<double>(step), static_cast<double>(stepFast), format, flags);
	}
	// D3DXVECTOR2�^�ŕ\��.
	else if constexpr (std::is_same_v<T, D3DXVECTOR2>) 
	{
		return ImGui::InputFloat2(newlabel.c_str(), &value.x, format, flags);
	}
	// D3DXVECTOR3�^�ŕ\��.
	else if constexpr (std::is_same_v<T, D3DXVECTOR3>) 
	{
		return ImGui::InputFloat3(newlabel.c_str(), &value.x, format, flags);
	}
	// string�^�ŕ\��.
	//���char�z��ɕϊ�������A���͂̎�t.
	else if constexpr (std::is_same_v<T, std::string>) 
	{
		char buffer[256];
		strncpy_s(buffer, value.c_str(), sizeof(buffer));
		if (ImGui::InputText(newlabel.c_str(), buffer, sizeof(buffer))) 
		{
			value = buffer;
			return true;
		}
	}
	return false;
}
//�Öق̃C���X�^���X��.
//�����I�C���X�^���X���F�e���v���[�g�֐������ۂɎg����悤�ɂ��邽��.
//����ŁA�Q�[�����̂�����ϐ������A���^�C����GUI����ҏW�ł���悤�ɂȂ�.
template bool CImguiManager::Input<int>			(const char*, int&,			bool, float, float, const char*, ImGuiInputTextFlags);
template bool CImguiManager::Input<float>		(const char*, float&,		bool, float, float, const char*, ImGuiInputTextFlags);
template bool CImguiManager::Input<double>		(const char*, double&,		bool, float, float, const char*, ImGuiInputTextFlags);
template bool CImguiManager::Input<D3DXVECTOR2>	(const char*, D3DXVECTOR2&, bool, float, float, const char*, ImGuiInputTextFlags);
template bool CImguiManager::Input<D3DXVECTOR3>	(const char*, D3DXVECTOR3&, bool, float, float, const char*, ImGuiInputTextFlags);
template bool CImguiManager::Input<std::string>	(const char*, std::string&, bool, float, float, const char*, ImGuiInputTextFlags);

//�X���C�_�[�̕\��.
template<typename T>
void CImguiManager::Slider(const char* label, T& value, T valueMin, T valueMax, bool isLabel)
{
	//���x��(������)�̕�������쐬
	const std::string newlabel = "##" + std::string(label);

	//���x��(������)�ݒ肪�L���Ȃ烉�x����\��.
	if (isLabel == true)
	{
		ImGui::Text(label);					//���x��(������)�̕\��.
		ImGui::SameLine(SAMALINE_OFFSET);	//���x��(������)�̉��ɃX���C�_�[��\��.
	}

	//int�^�ŕ\��.
	if constexpr (std::is_same_v<T, int>) 
	{
		ImGui::SliderInt(newlabel.c_str(), &value, valueMin, valueMax);
	}
	//float�^�ŕ\��.
	else if constexpr (std::is_same_v<T, float>) 
	{
		ImGui::SliderFloat(newlabel.c_str(), &value, valueMin, valueMax);
	}
}
//�Öق̃C���X�^���X��.
//�����I�C���X�^���X���F�e���v���[�g�֐������ۂɎg����悤�ɂ��邽��.
//����ŁA�Q�[�����̂�����ϐ������A���^�C����GUI����ҏW�ł���悤�ɂȂ�.
template void CImguiManager::Slider<int>(const char*, int&, int, int, bool);
template void CImguiManager::Slider<float>(const char*, float&, float, float, bool);

//�R���{�{�b�N�X.
//�����̑I��������1��I��UI�����.
std::string CImguiManager::Combo(const char* Label, std::string& NowItem, const std::vector<std::string>& List, bool isLabel, float space)
{
	//�I�𒆂̃��X�g�̗v�f�ԍ�.
	int No = 0;	

	//���X�g�̍ő吔���擾.
	const int List_Max = static_cast<int>(List.size());

	//���ݑI������Ă���List�̗v�f�ԍ���ۑ�.
	for (int i = 0; i < List_Max; i++) {
		if (List[i] == NowItem) {
			No = i;
			break;
		}
	}

	//���x���ݒ肪�L���Ȃ�e�L�X�g��\��.
	if (isLabel == true)
	{
		ImGui::Text(Label);
		ImGui::SameLine(space);
	}

	//�R���{�{�b�N�X�̕\��.
	if (ImGui::BeginCombo(std::string("##" + std::string(Label)).c_str(), NowItem.c_str()))
	{
		const int Size = static_cast<int>(List.size());
		for (int i = 0; i < Size; i++)
		{
			//�I����Ԃ��擾.
			bool is_Selected = (NowItem == List[i]);

			//�I�𒆂̃��X�g�̗v�f�ԍ����X�V����.
			if (ImGui::Selectable(List[i].c_str(), is_Selected)) { No = i; }

			//�I�����ꂽList�Ƀt�H�[�J�X��ݒ�.
			if (is_Selected) { ImGui::SetItemDefaultFocus(); }
		}
		ImGui::EndCombo();
	}

	return NowItem = List[No];
}

//�`�F�b�N�{�b�N�X�̕\��.
//On/Off�̐؂�ւ��X�C�b�`�����.
bool CImguiManager::CheckBox(const char* label, bool& flag, bool isLabel)
{

	// ���x���ݒ肪�L���Ȃ�e�L�X�g��\��.
	if (isLabel == true)
	{
		ImGui::Text(label);
		ImGui::SameLine(SAMALINE_OFFSET);
	}
	//flag�̒l�𒼐ڐ؂�ւ���.
	return ImGui::Checkbox(std::string("##" + std::string(label)).c_str(), &flag);
}

//�O���t��\��.
//�܂���O���t�Ńf�[�^�̐��ڂ�����.
void CImguiManager::Graph(const char* Label, std::vector<float>& Data, const ImVec2& Size)
{
	//std::string�FLabel��const char*�Ȃ̂ŁA�u+�v�ŕ����񌋍��ł��Ȃ�.
	//�Ȃ̂ŁAstd::string(Label)�ɂ��Ă���"##"�Ƃ��Č������Ă�.
	//ImGui::PlotLines�FDear ImGui�ɗp�ӂ���Ă���u�܂���O���t�`��p�֐��v.
	//					���l�̔z���n���ƁA��������ɕ��א܂���O���t�Ƃ��ĕ`��.
	//.c_str()�F��C������(const char)*�ɕϊ���AImGui::PlotLines�ɓn���Ă���.
	//�܂Ƃ߁F�n����Data�̐��l�z���܂���O���t�Ƃ��ĕ`�悷����Ă���.
	ImGui::PlotLines(std::string("##" + std::string(Label)).c_str(), Data.data(), static_cast<int>(Data.size()), 0, nullptr, FLT_MAX, FLT_MAX, Size);
}