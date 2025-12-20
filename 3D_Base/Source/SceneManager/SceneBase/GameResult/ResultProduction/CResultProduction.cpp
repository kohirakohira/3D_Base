#include "CResultProduction.h"
#include "../../../../Assets/DirectX/DirectX11/CDirectX11.h"

CResultProduction::CResultProduction()
	: m_Camera				( nullptr )
	, m_BodyMesh			( )
	, m_CannonMesh			( )

	, m_SpriteBackGround	( nullptr )
	, m_SpriteGround		( nullptr )
	, m_SpriteNumber		( nullptr )
	, m_SpriteKillUI		( nullptr )
	, m_SpritePlayerUI		( )

	, m_BackGround			( nullptr )
	, m_SpriteObjGround		( nullptr )
	, m_CharacterManager	( nullptr )

	, m_Number				( nullptr )
	, m_KillUI				( nullptr )
	, m_PlayerUI			( )

	, m_CharaPosX			( -75.0f )
	, m_IsJudge				( false )
{
}

CResultProduction::~CResultProduction()
{
}

//勝ち抜け.
void CResultProduction::WinUpdate()
{
	//ビュー・プロジェクションの更新.
	m_Camera->Update();

	//1位のデータ.
	auto [playerID, Kill] = CGameDataManager::GetInstance().GetTopCharacter();
	m_Number->SetNumber(Kill, 2);
	m_Number->Update();
	//勝ちか引き分けでの位置決め.
	SetPositionJudge(playerID);

}

void CResultProduction::WinDraw()
{
	//1位のデータ.
	auto [playerID, Kill] = CGameDataManager::GetInstance().GetTopCharacter();
	//背景描画.
	m_BackGround->Draw(m_Camera->m_mView, m_Camera->m_mProj);
	//地面描画.
	m_SpriteObjGround->Draw(m_Camera->m_mView, m_Camera->m_mProj);
	//キャラクターの表示.
	m_CharacterManager->Draw(m_Camera->m_mView, m_Camera->m_mProj, m_Camera->m_Light, m_Camera->m_Camera);
	//キル数の表示.
	m_Number->Draw();
	//キルUIの表示.
	m_KillUI->Draw();
	//キャラクター番号の表示.
	m_PlayerUI[playerID]->Draw();

}

void CResultProduction::DrawUpdate()
{
	//ビュー・プロジェクションの更新.
	m_Camera->Update();

	//最大キル数表示.
	auto [playerID, Kill] = CGameDataManager::GetInstance().GetTopCharacter();
	m_Number->SetNumber(Kill, 2);
	m_Number->Update();
	//勝ちか引き分けでの位置決め.
	SetPositionJudge(playerID);

}

void CResultProduction::DrawDraw()
{
	//背景描画.
	m_BackGround->Draw(m_Camera->m_mView, m_Camera->m_mProj);
	//地面描画.
	m_SpriteObjGround->Draw(m_Camera->m_mView, m_Camera->m_mProj);
	//キャラクターの表示.
	m_CharacterManager->Draw(m_Camera->m_mView, m_Camera->m_mProj, m_Camera->m_Light, m_Camera->m_Camera);
	//キル数の表示.
	m_Number->Draw();
	//キルUIの表示.
	m_KillUI->Draw();

}

//生成関数.
void CResultProduction::Create()
{
	//インスタンス生成(オブジェクト).
	m_BackGround		= std::make_unique<BackGround>();
	m_SpriteObjGround	= std::make_unique<CSpriteObject>();
	m_Camera			= std::make_shared<CCamera>();
	m_CharacterManager	= std::make_shared<CCharacterManager>();
	//キャラクターの生成.
	m_CharacterManager->Init();

	//スプライトの生成.
	m_SpriteBackGround	= std::make_shared<CSprite3D>();
	m_SpriteGround		= std::make_shared<CSprite3D>();
	m_SpriteNumber		= std::make_shared<CSprite2D>();
	m_SpriteKillUI		= std::make_shared<CSprite2D>();

	//スタティックメッシュの生成.
	for (int index = 0; index < PLAYER_MAX; index++)
	{
		//インスタンスの生成.
		m_BodyMesh.push_back(std::make_shared<CStaticMesh>());
		m_CannonMesh.push_back(std::make_shared<CStaticMesh>());
	}

	//数字クラスの生成.
	m_Number			= std::make_unique<NumberImage>();
	//キルUIの生成.
	m_KillUI			= std::make_unique<CUIObject>();

	//キャラクター番号.
	for (int index = 0; index < PLAYER_MAX; index++)
	{
		m_PlayerUI[index]		= std::make_unique<CUIObject>();
		m_SpritePlayerUI[index] = std::make_shared<CSprite2D>();
	}

}

//初期化.
void CResultProduction::Init()
{
	//カメラの初期化.
	m_Camera->Init();
	//カメラの設定.
	m_Camera->SetLightPos(-15.0f, 80.0f, -15.0f);	//位置設定.
	m_Camera->SetLightColor(1.0f, 1.0f, 1.0f);		//色の設定.
	m_Camera->SetLightIntensity(500.0f);			//ライトの強さ.
	m_Camera->SetLightRange(5000.0f);				//ライトの長さ.
	m_Camera->SetLightAtten(0.0f, 0.0f, 0.1f);		//ライトの減衰.
	m_Camera->SetTargetPos(D3DXVECTOR3(0.0f, 50.0f, -50.0f));

	//地面の設定.
	D3DXVECTOR3 angle = { 0.0f, 0.0f, 0.0f };
	angle.x = 90.0f * D3DX_PI / 180.0f;
	m_SpriteObjGround->SetRotation(angle);
	m_SpriteObjGround->SetPosition(0.0f, 0.0f, 0.0f);
	m_SpriteObjGround->SetScale(10.0f, 10.0f, 1.0f);

	m_BackGround->SetPosition(0.0f, 200.0f, 1000.0f);
	m_BackGround->SetRotation(0.0f, 0.0f, 0.0f);
	m_BackGround->SetScale(4.0f, 1.0f, 0.0f);

	//数字の情報.
	D3DXVECTOR2 KillPos = { WND_W / 2 + 16, 310 };
	m_Number->SetBasePosition(KillPos);
	m_Number->SetDigitWidth(32);
	m_Number->SetPosition(0.0f, 0.0f, 0.0f);
	m_Number->SetRotation(0.0f, 0.0f, 0.0f);
	m_Number->SetScale(1.0f, 1.0f, 1.0f);

	//キルUIの情報.
	m_KillUI->SetPosition(WND_W / 2 - 120.0f, WND_H / 2 - 300.0f, 0.0f);
	m_KillUI->SetRotation(0.0f, 0.0f, 0.0f);
	m_KillUI->SetScale(0.5f, 0.5f, 0.5f);

	//キャラクターUIの情報.
	for (int index = 0; index < PLAYER_MAX; index++)
	{
		m_PlayerUI[index]->SetPosition(500.0f, 90.0f, 0.0f);
		m_PlayerUI[index]->SetRotation(0.0f, 0.0f, 0.0f);
		m_PlayerUI[index]->SetScale(0.7f, 0.7f, 0.7f);
	}

}

//読み込み関数.
HRESULT CResultProduction::LoadData()
{
	//スプライト3Dの背景サイズ.
	CSprite3D::SPRITE_STATE BACKGROUND =
	{
		WND_W, WND_H,
		WND_W, WND_H,
		WND_W, WND_H
	};
	//スプライト3Dの地面サイズ.
	CSprite3D::SPRITE_STATE GROUND =
	{
		256, 256,
		256, 256,
		256, 256
	};
	//スプライト2Dの数字サイズ.
	CSprite2D::SPRITE_STATE NUMSIZE =
	{
		33, 32,			//描画幅,高さ..
		330, 32,		//元画像の幅,高さ..
		33, 32			//アニメーションをしないので、0でいい..
	};
	//スプライト2DのキルUIサイズ.
	CSprite2D::SPRITE_STATE KILLUISIZE =
	{
		256, 256,		//描画幅,高さ..
		256, 256,		//元画像の幅,高さ..
		256, 256		//アニメーションをしないので、0でいい..
	};
	//スプライト2DのキャラクターUIサイズ.
	CSprite2D::SPRITE_STATE CHARAUISIZE =
	{
		256, 256,		//描画幅,高さ..
		256, 256,		//元画像の幅,高さ..
		256, 256		//アニメーションをしないので、0でいい..
	};

	//スプライトの読み込み.
	m_SpriteBackGround->Init(CDirectX11::GetInstance(), _T("Data//Mesh//Static//OutBackImage//BackImg.png"), BACKGROUND);
	m_SpriteGround->Init(CDirectX11::GetInstance(), _T("Data\\Mesh\\Static\\Ground\\groundex.bmp"), GROUND);
	m_SpriteNumber->Init(_T("Data\\Texture\\UI\\Timer\\number.png"), NUMSIZE, false);
	m_SpriteKillUI->Init(_T("Data\\Texture\\UI\\KillNum_remake.png"), KILLUISIZE, false);
	for (int index = 0; index < PLAYER_MAX; index++)
	{
		switch (index)
		{
		case 0 :
			m_SpritePlayerUI[index]->Init(_T("Data\\Texture\\UI\\PlayerNumber\\OneP_ver2.png"), CHARAUISIZE, false);
			break;
		case 1 :
			m_SpritePlayerUI[index]->Init(_T("Data\\Texture\\UI\\PlayerNumber\\TwoP_ver2.png"), CHARAUISIZE, false);
			break;
		case 2 :
			m_SpritePlayerUI[index]->Init(_T("Data\\Texture\\UI\\PlayerNumber\\TreeP_ver2.png"), CHARAUISIZE, false);
			break;
		case 3 :
			m_SpritePlayerUI[index]->Init(_T("Data\\Texture\\UI\\PlayerNumber\\FourP_ver2.png"), CHARAUISIZE, false);
			break;
		default:
			break;
		}
	}

	//背景画像のアタッチ.
	m_BackGround->AttachSprite(*m_SpriteBackGround);
	//地面画像のアタッチ.
	m_SpriteObjGround->AttachSprite(*m_SpriteGround);
	//数字画像のアタッチ.
	m_Number->AttachSprite(m_SpriteNumber);
	//キルUI.
	m_KillUI->AttachSprite(m_SpriteKillUI);
	//キャラクターUI.
	for (int index = 0; index < PLAYER_MAX; index++)
	{
		m_PlayerUI[index]->AttachSprite(m_SpritePlayerUI[index]);
	}

	//画像の読み込み.
	for (int index = 0; index < PLAYER_MAX; index++)
	{
		switch (index)
		{
		case 0 :
			m_BodyMesh[index]->Init(  _T("Data\\Mesh\\Static\\Tank_n\\Red\\Body.x"));
			m_CannonMesh[index]->Init(_T("Data\\Mesh\\Static\\Tank_n\\Red\\Cannon.x"));
			break;
		case 1 :
			m_BodyMesh[index]->Init(  _T("Data\\Mesh\\Static\\Tank_n\\Yellow\\Body.x"));
			m_CannonMesh[index]->Init(_T("Data\\Mesh\\Static\\Tank_n\\Yellow\\Cannon.x"));
			break;
		case 2 :
			m_BodyMesh[index]->Init(  _T("Data\\Mesh\\Static\\Tank_n\\Green\\Body.x"));
			m_CannonMesh[index]->Init(_T("Data\\Mesh\\Static\\Tank_n\\Green\\Cannon.x"));
			break;
		case 3 :
			m_BodyMesh[index]->Init(  _T("Data\\Mesh\\Static\\Tank_n\\Blue\\Body.x"));
			m_CannonMesh[index]->Init(_T("Data\\Mesh\\Static\\Tank_n\\Blue\\Cannon.x"));
			break;
		default:
			break;
		}

		//画像のアタッチ.
		m_CharacterManager->AttachMeshesToPlayer(index, m_BodyMesh[index], m_CannonMesh[index]);
	}

	return S_OK;
}

//勝った時と引き分け時の位置設定.
void CResultProduction::SetPositionJudge(int playerid)
{
	if (m_IsJudge == true)
	{
		//キャラクターの情報.
		for (int index = 0; index < PLAYER_MAX; index++)
		{
			if (index == playerid)
			{
				D3DXVECTOR3 pos = { 0.0f, 10.0f, 80.0f };
				D3DXVECTOR3 rot = { 0.0f, D3DXToRadian(180), 0.0f };
				D3DXVECTOR3 sca = { 50.0f, 50.0f, 50.0f };
				m_CharacterManager->SetPlayerPosition(playerid, pos);
				m_CharacterManager->SetPlayerRotation(playerid, rot);
				m_CharacterManager->SetPlayerScale(playerid, sca);
			}
			else
			{
				D3DXVECTOR3 pos = { m_CharaPosX, 10.0f, 500.0f };
				D3DXVECTOR3 rot = { 0.0f, D3DXToRadian(180), 0.0f };
				D3DXVECTOR3 sca = { 50.0f, 50.0f, 50.0f };
				m_CharacterManager->SetPlayerPosition(index, pos);
				m_CharacterManager->SetPlayerRotation(index, rot);
				m_CharacterManager->SetPlayerScale(index, sca);
			}
			m_CharaPosX += 50.0f;
		}
	}
	else
	{
		//キャラクターの情報.
		for (int index = 0; index < PLAYER_MAX; index++)
		{
			D3DXVECTOR3 pos = { m_CharaPosX, 10.0f, 100.0f };
			D3DXVECTOR3 rot = { 0.0f, D3DXToRadian(180), 0.0f };
			D3DXVECTOR3 sca = { 50.0f, 50.0f, 50.0f };
			m_CharacterManager->SetPlayerPosition(index, pos);
			m_CharacterManager->SetPlayerRotation(index, rot);
			m_CharacterManager->SetPlayerScale(index, sca);
			m_CharaPosX += 50.0f;
		}
	}
	m_CharaPosX = -75.0f;

}
