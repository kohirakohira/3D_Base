#include "CResultProduction.h"
#include "../../../../Assets/DirectX/DirectX11/CDirectX11.h"

CResultProduction::CResultProduction()
	: m_Camera				( nullptr )
	, m_BodyMesh			( )
	, m_CannonMesh			( )

	, m_SpriteBackGround	( nullptr )
	, m_SpriteGround		( nullptr )

	, m_BackGround			( nullptr )
	, m_SpriteObjGround		( nullptr )
	, m_CharacterManager	( nullptr )

	, m_CharaPosX			( -75.0f )
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
}

void CResultProduction::WinDraw()
{
	//背景描画.
	m_BackGround->Draw(m_Camera->m_mView, m_Camera->m_mProj);
	//地面描画.
	m_SpriteObjGround->Draw(m_Camera->m_mView, m_Camera->m_mProj);
	//キャラクターの表示.
	m_CharacterManager->Draw(m_Camera->m_mView, m_Camera->m_mProj, m_Camera->m_Light, m_Camera->m_Camera);
}

void CResultProduction::DrawUpdate()
{
	//ビュー・プロジェクションの更新.
	m_Camera->Update();


}

void CResultProduction::DrawDraw()
{
	//背景描画.
	m_BackGround->Draw(m_Camera->m_mView, m_Camera->m_mProj);
	//地面描画.
	m_SpriteObjGround->Draw(m_Camera->m_mView, m_Camera->m_mProj);
	//キャラクターの表示.
	m_CharacterManager->Draw(m_Camera->m_mView, m_Camera->m_mProj, m_Camera->m_Light, m_Camera->m_Camera);

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

	//スタティックメッシュの生成.
	for (int index = 0; index < PLAYER_MAX; index++)
	{
		//インスタンスの生成.
		m_BodyMesh.push_back(std::make_shared<CStaticMesh>());
		m_CannonMesh.push_back(std::make_shared<CStaticMesh>());
	}

}

//初期化.
void CResultProduction::Init()
{
	//キャラクターの情報.
	for (int index = 0; index < PLAYER_MAX; index++)
	{
		D3DXVECTOR3 pos = { m_CharaPosX, 10.0f, 100.0f };
		D3DXVECTOR3 rot = { 0.0f, D3DXToRadian(180), 0.0f};
		D3DXVECTOR3 sca = { 50.0f, 50.0f, 50.0f };
		m_CharacterManager->SetPlayerPosition(index, pos);
		m_CharacterManager->SetPlayerRotation(index, rot);
		m_CharacterManager->SetPlayerScale(index, sca);
		m_CharaPosX += 50.0f;
	}

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

	//スプタイトの読み込み.
	m_SpriteBackGround->Init(CDirectX11::GetInstance(), _T("Data//Mesh//Static//OutBackImage//BackImg.png"), BACKGROUND);
	m_SpriteGround->Init(CDirectX11::GetInstance(), _T("Data\\Mesh\\Static\\Ground\\groundex.bmp"), GROUND);

	//背景画像のアタッチ.
	m_BackGround->AttachSprite(*m_SpriteBackGround);
	//地面画像のアタッチ.
	m_SpriteObjGround->AttachSprite(*m_SpriteGround);


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
			m_BodyMesh[index]->Init(  _T("Data\\Mesh\\Static\\Tank_n\\Blue\\Body.x"));
			m_CannonMesh[index]->Init(_T("Data\\Mesh\\Static\\Tank_n\\Blue\\Cannon.x"));
			break;
		case 3 :
			m_BodyMesh[index]->Init(  _T("Data\\Mesh\\Static\\Tank_n\\Green\\Body.x"));
			m_CannonMesh[index]->Init(_T("Data\\Mesh\\Static\\Tank_n\\Green\\Cannon.x"));
			break;
		default:
			break;
		}

		//画像のアタッチ.
		m_CharacterManager->AttachMeshesToPlayer(index, m_BodyMesh[index], m_CannonMesh[index]);
	}

	return S_OK;
}
