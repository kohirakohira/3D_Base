#include "CResultProduction.h"
#include "../../../../Assets/DirectX/DirectX11/CDirectX11.h"

//デルタタイム.
const float dt = 1.0f / FPS;

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
	, m_Timer				( 0.0f )

	, m_StagingPosition		()
	, m_Result				()

	, m_CharacterEffects	()
	, m_FireworkEffects		()
{
}

CResultProduction::~CResultProduction()
{
	//中を消す.
	m_CharacterEffects.clear();
	m_FireworkEffects.clear();
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

	//色の設定.
	Effekseer::Color col = { 255, 255, 255, 255 };

	//エフェクトの時間.
	m_Timer += dt;

	//エフェクト.
	for (int i = 0; i < m_FireworkEffects.size(); i++)
	{
		//キャラ位置の取得.
		D3DXVECTOR3 firePos = { 0.0f, 0.0f, 0.0f };

		//エフェクト処理.
		for (auto& effect : m_FireworkEffects[i].effects)
		{
			if (m_Timer >= 3.0f)
			{
				CEffect::GetInstance().Stop(effect.handle);
				effect.handle = -1;
				m_Timer = 0.0f;
			}
			//再生されていないとき.
			if (effect.handle == -1)
			{
				//キャラ位置 + オフセット = エフェクト位置.
				D3DXVECTOR3 worldPos =
				{
					firePos.x + effect.offset.x,
					firePos.y + effect.offset.y,
					firePos.z + effect.offset.z
				};

				//再生.
				effect.handle = CEffect::GetInstance().Play(CEffect::Firework, worldPos);
				//エフェクトの回転を設定.
				CEffect::GetInstance().SetRotation(effect.handle, D3DXVECTOR3{ 0.0f, 0.0f, 0.0f });
				//エフェクトのサイズを設定.
				CEffect::GetInstance().SetScale(effect.handle, D3DXVECTOR3{ 3.0f, 3.0f, 3.0f });
				//エフェクトの位置を設定.
				CEffect::GetInstance().SetLocation(effect.handle, worldPos);
				//色とα値を設定.
				CEffect::GetInstance().SetAlpha(effect.handle, col);
			}

		}

	}
}

void CResultProduction::WinDraw()
{
	//エフェクトの描画.
	CEffect::GetInstance().Draw(m_Camera->m_mView, m_Camera->m_mProj, m_Camera->m_Light, m_Camera->m_Camera);
	
	//1位のデータ.
	auto [playerID, Kill] = CGameDataManager::GetInstance().GetTopCharacter();
	//背景描画.
	m_BackGround->Draw(m_Camera->m_mView, m_Camera->m_mProj);
	//地面描画.
	m_SpriteObjGround->Draw(m_Camera->m_mView, m_Camera->m_mProj);
	//キャラクターの表示.
	m_CharacterManager->DrawResult(m_Result.players, m_Camera->m_mView, m_Camera->m_mProj, m_Camera->m_Light, m_Camera->m_Camera);
	
	CDirectX11::GetInstance().SetDepth(false);
	//キル数の表示.
	m_Number->Draw();
	//キルUIの表示.
	m_KillUI->Draw();
	//キャラクター番号の表示.
	m_PlayerUI[playerID]->Draw();
	CDirectX11::GetInstance().SetDepth(true);

}

void CResultProduction::DrawUpdate()
{
	//色の設定.
	Effekseer::Color col = { 255, 255, 255, 255 };

	//ビュー・プロジェクションの更新.
	m_Camera->Update();

	//最大キル数表示.
	auto [playerID, Kill] = CGameDataManager::GetInstance().GetTopCharacter();
	m_Number->SetNumber(Kill, 2);
	m_Number->Update();

	//エフェクトの時間.
	m_Timer += dt;

	//エフェクト.
	for (int i = 0; i < m_CharacterEffects.size(); i++)
	{
		//プレイヤーID.
		int playerID = m_Result.players[i];

		//キャラ位置の取得.
		D3DXVECTOR3 charPos = m_CharacterManager->GetPosition(playerID);

		//エフェクト処理.
		for (auto& effect : m_CharacterEffects[i].effects)
		{
			//再生されていないとき.
			if (effect.handle == -1)
			{
				//キャラ位置 + オフセット = エフェクト位置.
				D3DXVECTOR3 worldPos =
				{
					charPos.x + effect.offset.x,
					charPos.y + effect.offset.y,
					charPos.z + effect.offset.z
				};

				//再生.
				effect.handle = CEffect::GetInstance().Play(CEffect::Smoke, worldPos);
				//エフェクトの回転を設定.
				CEffect::GetInstance().SetRotation(effect.handle, D3DXVECTOR3{ D3DXToRadian(25.0f), 0.0f, 0.0f });
				//エフェクトのサイズを設定.
				CEffect::GetInstance().SetScale(effect.handle, D3DXVECTOR3{ 3.0f, 3.0f, 3.0f });
				//エフェクトの位置を設定.
				CEffect::GetInstance().SetLocation(effect.handle, worldPos);
				//色とα値を設定.
				CEffect::GetInstance().SetAlpha(effect.handle, col);
			}
			if (m_Timer >= 5.0f)
			{
				CEffect::GetInstance().Stop(effect.handle);
				effect.handle = -1;
				m_Timer = 0.0f;
			}
		}

	}
}

void CResultProduction::DrawDraw()
{
	//エフェクトの描画.
	CEffect::GetInstance().Draw(m_Camera->m_mView, m_Camera->m_mProj, m_Camera->m_Light, m_Camera->m_Camera);

	//背景描画.
	m_BackGround->Draw(m_Camera->m_mView, m_Camera->m_mProj);
	//地面描画.
	m_SpriteObjGround->Draw(m_Camera->m_mView, m_Camera->m_mProj);
	//キャラクターの表示.
	m_CharacterManager->DrawResult(m_Result.players, m_Camera->m_mView, m_Camera->m_mProj, m_Camera->m_Light, m_Camera->m_Camera);

	CDirectX11::GetInstance().SetDepth(false);
	//キル数の表示.
	m_Number->Draw();
	//キルUIの表示.
	m_KillUI->Draw();
	CDirectX11::GetInstance().SetDepth(true);

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
void CResultProduction::Init(DrawResult result)
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

	//プレイヤー番号の情報.
	for (int index = 0; index < PLAYER_MAX; index++)
	{
		m_PlayerUI[index]->SetPosition(WND_W / 2 - 450, 100.0f, 0.0f);
		m_PlayerUI[index]->SetRotation(0.0f, 0.0f, 0.0f);
		m_PlayerUI[index]->SetScale(0.7f, 0.7f, 0.7f);
	}

	if (m_IsJudge != true)
	{
		//キャラクターの位置設定(順位).
		m_StagingPosition.OnePos	= { -65.0f,	10.0f, 100.0f };	//中央.
		m_StagingPosition.TwoPos	= { -20.0f, 10.0f, 100.0f };	//左奥.
		m_StagingPosition.TreePos	= { 25.0f,	10.0f, 100.0f };	//右の左奥.
		m_StagingPosition.Fouros	= { 70.0f,	10.0f, 100.0f };	//右の右奥.
	}
	else
	{
		//キャラクターの位置設定(順位).
		m_StagingPosition.OnePos	= { 0.0f,	10.0f, 80.0f };		//中央.
		m_StagingPosition.TwoPos	= { -50.0f, 10.0f, 150.0f };	//左奥.
		m_StagingPosition.TreePos	= { 50.0f,	10.0f, 150.0f };	//右の左奥.
		m_StagingPosition.Fouros	= { 95.0f,	10.0f, 150.0f };	//右の右奥.
	}

	//リザルト結果を保存.
	m_Result = result;

	//キャラ分演出管理を用意.
	m_CharacterEffects.resize(m_Result.players.size());
	//花火演出管理を用意※4つ用意.
	m_FireworkEffects.resize(4);

	//エフェクト.
	for (auto& charEff : m_CharacterEffects)
	{
		//出す位置.
		charEff.effects =
		{
			//ハンドル初期化, { x座標, y座標, z座標 }.
			{ -1, {-10.0f, -10.0f,  25.0f} },	//左後ろ.
			{ -1, { 15.0f, -10.0f, -25.0f} }	//右前.
		};
	}
	for (auto& fireEff : m_FireworkEffects)
	{
		//出す位置.
		fireEff.effects =
		{
			{ -1, {-180.0f,	 20.0f, 300.0f} },
			{ -1, { 180.0f,	 20.0f, 200.0f} },
			{ -1, {-110.0f,	-20.0f, 300.0f} },
			{ -1, { 110.0f,	-20.0f, 200.0f} }
		};
	}


	//勝ちか引き分けでの位置決め.
	if (m_Result.players.size() == 1)
	{
		SetPositionRanking();
	}
	else
	{
		SetPositionJudge(m_Result);
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

//引き分け時の位置設定.
void CResultProduction::SetPositionJudge(const DrawResult& result)
{
	//間隔.
	const float spacing = 50.0f;

	//回転と大きさ.
	const D3DXVECTOR3 rot = {0.0f, D3DXToRadian(180.0f), 0.0f};
	const D3DXVECTOR3 sca = { 50.0f, 50.0f, 50.0f };

	auto xs = CalcCenterPosition(result.players.size(), spacing);

	for (int i = 0; i < result.players.size(); i++)
	{
		int playerID = result.players[i];

		//位置.
		D3DXVECTOR3 pos = {xs[i], 10.0f, 100.0f};

		m_CharacterManager->SetPlayerPosition(playerID, pos);
		m_CharacterManager->SetPlayerRotation(playerID, rot);
		m_CharacterManager->SetPlayerScale(playerID, sca);

	}

}

//キャラクターの位置設定用.
void CResultProduction::SetPositionRanking()
{
	//順位を取得.
	auto ranks = CGameDataManager::GetInstance().GetRanking();

	//回転と大きさを設定.
	const D3DXVECTOR3 rot = { 0.0f, D3DXToRadian(180), 0.0f };
	const D3DXVECTOR3 sca = { 50.0f, 50.0f, 50.0f };

	//順位を見て位置を設定.
	for (int rank = 0; rank < PLAYER_MAX; ++rank)
	{
		//今の順位のプレイヤーIDを取得.
		int playerID = ranks[rank];
		//例外処理.
		if (playerID < 0)
		{
			continue;
		}
		//位置を保存する変数.
		D3DXVECTOR3 pos;

		//順位に応じて決めておいた位置を選ぶ※1位~4位.
		switch (rank)
		{
		case 0:
			pos = m_StagingPosition.OnePos;
			break;
		case 1:
			pos = m_StagingPosition.TwoPos;
			break;
		case 2:
			pos = m_StagingPosition.TreePos;
			break;
		case 3:
			pos = m_StagingPosition.Fouros;
			break;
		default:
			break;
		}
		//キャラクターに反映.
		m_CharacterManager->SetPlayerPosition(playerID, pos);
		m_CharacterManager->SetPlayerRotation(playerID, rot);
		m_CharacterManager->SetPlayerScale(playerID, sca);
	}
}

//キャラクターのX座標を決める関数.
std::vector<float> CResultProduction::CalcCenterPosition(int count, float spacing)
{
	//X座標を格納する変数.
	std::vector<float> result;

	//例外処理.
	if (count <= 0) return result;

	//開始位置の計算.
	float start = -(count - 1) * 0.5f * spacing;

	//X座標を順番に作る.
	for (int i = 0; i < count; i++)
	{
		result.push_back(start + i * spacing);
	}

	return result;
}
