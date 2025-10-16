#include "CImguiManager.h"
#include <utility>

constexpr char FONT_FILE_PATH[]		= "Data//ImGui//Font//NotoSansJP-SemiBold.ttf";
constexpr float FONT_SIZE			= 18.0f;
constexpr float SAMALINE_OFFSET		= 100.0f;//ImGui::SameLineのオフセット値.

CImguiManager::CImguiManager()
{
}

CImguiManager::~CImguiManager()
{
}

//初期化.
HRESULT CImguiManager::Init(HWND hWnd)
{
	//コンテキスト(ImGuiの世界)の作成.
	ImGui::CreateContext();

	//imgui.hの中にある構造体※ImGui の挙動や設定、入力状態 ...etc.
	//ゲームのウィンドウに ImGui を表示できるようにする準備↓.
	ImGuiIO io = ImGui::GetIO();
	//フレーム時間、ウィンドウサイズ、フラグなどの設定.
	//「|=」: ビットごとのOR演算子※わからない時は調べて.
	io.ConfigFlags		|= ImGuiConfigFlags_DockingEnable;		//ウィンドウをドラッグして合体・分割するのを有効にする.
	io.ConfigFlags		|= ImGuiConfigFlags_ViewportsEnable;	//ImGui のウィンドウをゲームウィンドウ外に出せる.
	io.DeltaTime		 = 1.0f / 60.0f;						//1フレームの時間.
	io.DisplaySize.x	 = WND_WF;								//横の画面サイズ.
	io.DisplaySize.x	 = WND_HF;								//縦の画面サイズ.

	//フォントの読み込み※フォントの切り替え、文字幅の計算、テキスト描画処理...etc.
	ImFont* font = io.Fonts->AddFontFromFileTTF(
		FONT_FILE_PATH,							//フォントファイルのパス.
		FONT_SIZE,								//フォントサイズ(ピクセル単位).
		NULL,									//フォントの詳細設定※今回は特に指定しない.
		io.Fonts->GetGlyphRangesJapanese());	//使う文字の範囲(グリフ範囲)指定※指定しないとASCIIくらいしか入らない。今回は日本語(がな、カナ、漢字一部)でも大丈夫.

	//ここで基本の色やフォントサイズなどが「ひな形」として決める↓.
	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsDefalt();
	ImGui::StyleColorsLight();

	//ウィンドウの丸みや色をカスタマイズできる用にする.
	ImGuiStyle style = ImGui::GetStyle();
	//UIウィンドウをメイン画面から切り離して別ウィンドウに移動できる機能がONになっている?
	//「&」: ビットごとのAND演算子※わからない時は調べて.
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 5.0f;				//ウィンドウの角を丸めず、フラットな四角にする.
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;	//背景色のアルファ(透明度)を100%(不透明)にする.
	}

	//Win32 プラットフォーム用バックエンドを初期化.
	//入力処理(マウス、キーボード、IME)をImGuiで使用できるようにする.
	if (ImGui_ImplWin32_Init(hWnd) == false)
	{
		return E_FAIL;
	}
	//DirectX11 レンダラーバックエンド を初期化.
	//自前の描画命令をDirectX11に投げられるようにする.
	if (ImGui_ImplDX11_Init(CDirectX11::GetInstance().GetDevice(), CDirectX11::GetInstance().GetContext()) == false)
	{
		return E_FAIL;
	}

	return S_OK;
}
//解放.
void CImguiManager::Release()
{
	ImGui_ImplDX11_Shutdown();		//DirectX11用バックエンドの終了処理.
	ImGui_ImplWin32_Shutdown();		//Win32用バックエンドの終了処理.
	ImGui::DestroyContext();		//ImGui全体のコンテキスト(世界)を破棄.
}

//フレーム設定.
void CImguiManager::SetFrame()
{
	ImGui_ImplDX11_NewFrame();		//DirectX11側の新フレーム準備.
	ImGui_ImplWin32_NewFrame();		//Win32側の新フレーム準備.
	ImGui::NewFrame();				//ImGui本体の新フレーム開始.
}

//描画.
void CImguiManager::Render()
{
	//ImGui::GetIO()：ImGuiの入力・出力設定をまとめた「IO(Input/Output)」オブジェクトを返す関数.
	//(void)io;：「この変数はわざと使ってません！」とコンパイラに伝える.
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//描画.
	ImGui::Render();
	//DirectX11に描画データを渡し、実際にGPUで描画させる.
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	//複数ウィンドウをサポートしている時に追加処理.
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();			//新しく開いたウィンドウの情報を更新.
		ImGui::RenderPlatformWindowsDefault();	//メインウィンドウ以外のImGuiウィンドウも描画.
	}
}

//入力ボックスの表示.
//型を後から指定できる関数↓.
template<typename T>
inline bool CImguiManager::Input(const char* label, T& value, bool isLabel, float step, float stepFast, const char* format, ImGuiInputTextFlags flags)
{
	//ラベル(説明文)の文字列を作成※"##"：ImGuiでID管理用.
	const std::string newlabel = "##" + std::string(label);

	//ラベル(説明文)設定が有効ならラベルを表示.
	if (isLabel == true)
	{
		ImGui::Text(label);					//ラベル(説明分)を左表示.
		ImGui::SameLine(SAMALINE_OFFSET);	//ラベルの横に入力欄表示.
	}

	//if constexpr：コンパイル時に型を判定して、必要な分岐だけを残す.
	// int型で表示.
	if constexpr (std::is_same_v<T, int>) 
	{
		return ImGui::InputInt(newlabel.c_str(), &value, static_cast<int>(step), static_cast<int>(stepFast), flags);
	}
	// float型で表示.
	else if constexpr (std::is_same_v<T, float>) 
	{
		return ImGui::InputFloat(newlabel.c_str(), &value, step, stepFast, format, flags);
	}
	// double型で表示.
	else if constexpr (std::is_same_v<T, double>) 
	{
		return ImGui::InputDouble(newlabel.c_str(), &value, static_cast<double>(step), static_cast<double>(stepFast), format, flags);
	}
	// D3DXVECTOR2型で表示.
	else if constexpr (std::is_same_v<T, D3DXVECTOR2>) 
	{
		return ImGui::InputFloat2(newlabel.c_str(), &value.x, format, flags);
	}
	// D3DXVECTOR3型で表示.
	else if constexpr (std::is_same_v<T, D3DXVECTOR3>) 
	{
		return ImGui::InputFloat3(newlabel.c_str(), &value.x, format, flags);
	}
	// string型で表示.
	//一回char配列に変換した後、入力の受付.
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
//暗黙のインスタンス化.
//明示的インスタンス化：テンプレート関数を実際に使えるようにするため.
//これで、ゲーム中のあらゆる変数をリアルタイムでGUIから編集できるようになる.
template bool CImguiManager::Input<int>			(const char*, int&,			bool, float, float, const char*, ImGuiInputTextFlags);
template bool CImguiManager::Input<float>		(const char*, float&,		bool, float, float, const char*, ImGuiInputTextFlags);
template bool CImguiManager::Input<double>		(const char*, double&,		bool, float, float, const char*, ImGuiInputTextFlags);
template bool CImguiManager::Input<D3DXVECTOR2>	(const char*, D3DXVECTOR2&, bool, float, float, const char*, ImGuiInputTextFlags);
template bool CImguiManager::Input<D3DXVECTOR3>	(const char*, D3DXVECTOR3&, bool, float, float, const char*, ImGuiInputTextFlags);
template bool CImguiManager::Input<std::string>	(const char*, std::string&, bool, float, float, const char*, ImGuiInputTextFlags);

//スライダーの表示.
template<typename T>
void CImguiManager::Slider(const char* label, T& value, T valueMin, T valueMax, bool isLabel)
{
	//ラベル(説明文)の文字列を作成
	const std::string newlabel = "##" + std::string(label);

	//ラベル(説明文)設定が有効ならラベルを表示.
	if (isLabel == true)
	{
		ImGui::Text(label);					//ラベル(説明文)の表示.
		ImGui::SameLine(SAMALINE_OFFSET);	//ラベル(説明文)の横にスライダーを表示.
	}

	//int型で表示.
	if constexpr (std::is_same_v<T, int>) 
	{
		ImGui::SliderInt(newlabel.c_str(), &value, valueMin, valueMax);
	}
	//float型で表示.
	else if constexpr (std::is_same_v<T, float>) 
	{
		ImGui::SliderFloat(newlabel.c_str(), &value, valueMin, valueMax);
	}
}
//暗黙のインスタンス化.
//明示的インスタンス化：テンプレート関数を実際に使えるようにするため.
//これで、ゲーム中のあらゆる変数をリアルタイムでGUIから編集できるようになる.
template void CImguiManager::Slider<int>(const char*, int&, int, int, bool);
template void CImguiManager::Slider<float>(const char*, float&, float, float, bool);

//コンボボックス.
//複数の選択肢から1つを選ぶUIを作る.
std::string CImguiManager::Combo(const char* Label, std::string& NowItem, const std::vector<std::string>& List, bool isLabel, float space)
{
	//選択中のリストの要素番号.
	int No = 0;	

	//リストの最大数を取得.
	const int List_Max = static_cast<int>(List.size());

	//現在選択されているListの要素番号を保存.
	for (int i = 0; i < List_Max; i++) {
		if (List[i] == NowItem) {
			No = i;
			break;
		}
	}

	//ラベル設定が有効ならテキストを表示.
	if (isLabel == true)
	{
		ImGui::Text(Label);
		ImGui::SameLine(space);
	}

	//コンボボックスの表示.
	if (ImGui::BeginCombo(std::string("##" + std::string(Label)).c_str(), NowItem.c_str()))
	{
		const int Size = static_cast<int>(List.size());
		for (int i = 0; i < Size; i++)
		{
			//選択状態を取得.
			bool is_Selected = (NowItem == List[i]);

			//選択中のリストの要素番号を更新する.
			if (ImGui::Selectable(List[i].c_str(), is_Selected)) { No = i; }

			//選択されたListにフォーカスを設定.
			if (is_Selected) { ImGui::SetItemDefaultFocus(); }
		}
		ImGui::EndCombo();
	}

	return NowItem = List[No];
}

//チェックボックスの表示.
//On/Offの切り替えスイッチを作る.
bool CImguiManager::CheckBox(const char* label, bool& flag, bool isLabel)
{

	// ラベル設定が有効ならテキストを表示.
	if (isLabel == true)
	{
		ImGui::Text(label);
		ImGui::SameLine(SAMALINE_OFFSET);
	}
	//flagの値を直接切り替える.
	return ImGui::Checkbox(std::string("##" + std::string(label)).c_str(), &flag);
}

//グラフを表示.
//折れ線グラフでデータの推移を可視化.
void CImguiManager::Graph(const char* Label, std::vector<float>& Data, const ImVec2& Size)
{
	//std::string：Labelはconst char*なので、「+」で文字列結合できない.
	//なので、std::string(Label)にしてから"##"として結合してる.
	//ImGui::PlotLines：Dear ImGuiに用意されている「折れ線グラフ描画用関数」.
	//					数値の配列を渡すと、それを横に並べ折れ線グラフとして描画.
	//.c_str()：でC文字列(const char)*に変換後、ImGui::PlotLinesに渡している.
	//まとめ：渡したDataの数値配列を折れ線グラフとして描画するってこと.
	ImGui::PlotLines(std::string("##" + std::string(Label)).c_str(), Data.data(), static_cast<int>(Data.size()), 0, nullptr, FLT_MAX, FLT_MAX, Size);
}