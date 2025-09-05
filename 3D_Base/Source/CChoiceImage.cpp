//宣言して短くしている.
#define GetKey(KEY) (GetAsyncKeyState(KEY))

#include "CChoiceImage.h"

CChoiceImage::CChoiceImage(CSceneType typ)
	:m_SceneType		( typ )

	, m_IsSelected		( false )
{

}

CChoiceImage::~CChoiceImage()
{

}

//動作関数.
void CChoiceImage::Update()
{
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
		if (GetKey('W') & 0x0001)
		{
			//戻る処理.
			m_vPosition.y = WND_H / posAdjustment_1;
			//セッティング.
			m_IsSelected = false;
		}
		//下移動(エンド).
		if (GetKey('S') & 0x0001)
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
		if (GetKey('D') & 0x0001)
		{
			//タイトルに戻る処理.
			m_vPosition.x = WND_W / posAdjustment_3;
			//戻る.
			m_IsSelected = false;
		}
		//左移動(戻る).
		if (GetKey('A') & 0x0001)
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
