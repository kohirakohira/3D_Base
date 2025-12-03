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
	if (CControllerManager::GetInstance().GetController(0) != nullptr)
	{
		//コントローラーの動作.
		ControllerUpdate();
	}
	else
	{
		//キーの動作.
		KeyUpdate();
	}



}

void CChoiceImage::ControllerUpdate()
{
	//スティックの入力方向取得.
	CController::Direction dirlef = CControllerManager::GetInstance().GetController(0)->GetLeftStickDirection(0.2f);

	switch (m_SceneType)
	{
	case CSceneType::Title:
		//上移動(プレイ).
		if (dirlef == CController::Direction::Up)
		{
			//戻る処理.
			m_vPosition.y = WND_H / posAdjustment_1;
			//セッティング.
			m_IsSelected = false;
		}
		//下移動(エンド).
		if (dirlef == CController::Direction::Down)
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
		if (dirlef == CController::Direction::Right)
		{
			//タイトルに戻る処理.
			m_vPosition.x = WND_W / posAdjustment_3;
			//戻る.
			m_IsSelected = false;
		}
		//左移動(戻る).
		if (dirlef == CController::Direction::Left)
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

void CChoiceImage::KeyUpdate()
{
	switch (m_SceneType)
	{
	case CSceneType::Title:
		//上移動(プレイ).
		if (m_Key->NowInputKey('W') == true)
		{
			//戻る処理.
			m_vPosition.y = WND_H / posAdjustment_1;
			//セッティング.
			m_IsSelected = false;
		}
		//下移動(エンド).
		if (m_Key->NowInputKey('S') == true)
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
		if (m_Key->NowInputKey('D') == true)
		{
			//タイトルに戻る処理.
			m_vPosition.x = WND_W / posAdjustment_3;
			//戻る.
			m_IsSelected = false;
		}
		//左移動(戻る).
		if (m_Key->NowInputKey('A') == true)
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
