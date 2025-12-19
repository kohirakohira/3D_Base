#include "FakeLoad.h"

FakeLoad::FakeLoad()
	: m_SpriteObj			( nullptr )
	, m_Sprite2DFakeLoad	( nullptr )

	, m_IsDraw				( false )
	, m_Alpha				( 0.0f )
{
	//インスタンス生成.
	m_SpriteObj			= std::make_unique<CUIObject>();
	m_Sprite2DFakeLoad	= std::make_shared<CSprite2D>();
}
FakeLoad::~FakeLoad()
{
}

//更新処理.
void FakeLoad::Update()
{
	m_SpriteObj->Update();
	if (m_IsDraw == true)
	{
		if (m_Alpha <= 255.0f)
		{
			m_Alpha += 1.5f;
		}
		else
		{
			m_Alpha = 0.0f;
		}
	}
	else
	{
		m_Alpha = 0.0f;
	}
}

//描画処理.
void FakeLoad::Draw()
{
	if (m_IsDraw == true)
	{
		//色の設定.
		D3DXVECTOR4 color = { 1.0f, 1.0f, 1.0f, m_Alpha };
		m_SpriteObj->SetColor(color);
		m_SpriteObj->Draw();
	}
}

//初期化.
void FakeLoad::Init()
{
	//画像をアタッチ.
	m_SpriteObj->AttachSprite(m_Sprite2DFakeLoad);

	//ロード画像の情報.
	m_SpriteObj->SetPosition(0.0f, 0.0f, 0.0f);
	m_SpriteObj->SetRotation(0.0f, 0.0f, 0.0f);
	m_SpriteObj->SetScale(1.0f, 1.0f, 1.0f);

}

//素材の読み込み.
HRESULT FakeLoad::LoadData()
{
	//画像のサイズ.
	CSprite2D::SPRITE_STATE FAKELOAD =
	{
		WND_W, WND_H,
		WND_W, WND_H,
		WND_W, WND_H
	};

	//フェイクロード画像の読み込み.
	m_Sprite2DFakeLoad->Init(_T("Data\\Texture\\UI\\FakeLoad.png"), FAKELOAD, false);

	return S_OK;

}