#include "CChoiceImage.h"

CChoiceImage::CChoiceImage(CSceneType typ)
	:m_SceneType		( typ )

	, m_IsSelected		( false )
{
	//キーのインスタンス生成.
	m_Key = std::make_unique<CMultiInputKeyManager>();
	m_Key->Init();
	//必要なキーの設定.
	m_Key->SetKey({'W', 'A', 'S', 'D' });
}

CChoiceImage::~CChoiceImage()
{

}

//動作関数.
void CChoiceImage::Update()
{
	//キーを毎フレーム更新.
	m_Key->Update();

	//選択肢の移動関数.
	MoveChoiceImg();
}

//描画関数.
void CChoiceImage::Draw()
{
	//描画処理.
	CUIObject::Draw();
}

//選択肢の移動関数.
void CChoiceImage::MoveChoiceImg()
{
	//コントローラーの取得※0番しか動かせない.
	CController* controller = CControllerManager::GetInstance().GetController(0);
	////中身が無いときは通らないようにする.
	//if (!controller || !controller->CheckConnected()) return;

	//スティックの入力方向取得.
	CController::Direction dirlef = controller->GetLeftStickDirection(0.2f);

	//定数宣言.
	//位置の調整用.
	const float posAdjustment_1 = 1.5f;
	const float posAdjustment_2 = 1.2f;
	const float posAdjustment_3 = 1.37f;
	const float posAdjustment_4 = 2.75f;

	switch (m_SceneType)
	{
	case CSceneType::Title:
		//上移動(プレイ).
		if (m_Key->NowInputKey('W') || dirlef == CController::Direction::Up)
		{
			//戻る処理.
			m_vPosition.y = WND_H / posAdjustment_1;
			//セッティング.
			m_IsSelected = false;
		}
		//下移動(エンド).
		if (m_Key->NowInputKey('S') || dirlef == CController::Direction::Down)
		{
			//ゲームスタート処理.
			m_vPosition.y = WND_H / posAdjustment_2;
			//閉じる.
			m_IsSelected = true;
		}
		break;
	case CSceneType::Debug:
		//多分何も処理がない.
		break;
	case CSceneType::Setting:
		//右移動(スタート).
		if (m_Key->NowInputKey('D') || dirlef == CController::Direction::Right)
		{
			//タイトルに戻る処理.
			m_vPosition.x = WND_W / posAdjustment_3;
			//戻る.
			m_IsSelected = false;
		}
		//左移動(戻る).
		if (m_Key->NowInputKey('A') || dirlef == CController::Direction::Left)
		{
			//ゲームメイン処理.
			m_vPosition.x = WND_W / posAdjustment_4;
			//メイン.
			m_IsSelected = true;
		}
		break;
	case CSceneType::Main:
		//多分何も処理がない.
		break;
	case CSceneType::Result:
	case CSceneType::ResultWin:
	case CSceneType::ResultDraw:

		break;
	default:
		break;
	}
}
