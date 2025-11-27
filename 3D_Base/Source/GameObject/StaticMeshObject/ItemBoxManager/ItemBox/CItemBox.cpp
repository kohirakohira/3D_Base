#include "CItemBox.h"

CItemBox::CItemBox()
	: InitialSpeed			( 0.f )
	, GravitySpeed			( -9.8f )
	, m_Active				( true )
	, m_ItemType			()
	, IsGravity				( false )
	, ItemFlag				( false )
	, hEffect				( -1 )
{
	//大体0.016辺りになる.
	Framerate = 1.f / 60.f;

	m_pCollider = std::make_shared<CBoxCollider>();
}

CItemBox::~CItemBox()
{
}

void CItemBox::Update()
{
	//アイテムを落下させるための関数.
	GravityMath();

	if (ItemFlag == true)
	{
		//エフェクトを再生.
		hEffect = CEffect::Play(CEffect::Mist, m_vPosition);

		//拡大縮小
		CEffect::SetScale(hEffect, D3DXVECTOR3(1.0f, 1.0f, 1.0f));

		//回転(Y軸回転)
		CEffect::SetRotation(hEffect, D3DXVECTOR3(0.0f, 0.0f, 0.0f));

		//位置を再設定
		CEffect::SetLocation(hEffect, m_vPosition);

		CEffect::SetSpeed(hEffect, 0.1f);

		std::cout << hEffect << std::endl;

		ItemFlag = false;
	}
	//centerpos・rotation・scaleを設定してる.
	CStaticMeshObject::Update();
}

void CItemBox::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	if (m_Active == true)
	{
		CStaticMeshObject::Draw(View, Proj, Light, Camera);
	}
}

//重力の計算用(下に落下).
void CItemBox::GravityMath()
{
	if (IsGravity == false)
	{
		//位置変化(Y座標 += 初期速度 * 1フレーム + 0.5f * 重力加速度 * 1フレーム * 1フレーム)※0.5fは1/2のこと.
		//「0.5f * 重力加速度 * 1フレーム * 1フレーム」は「加速度によって物体がどれだけ移動するか」を表す式.
		//1フレーム * 1フレームは1フレームにかかる時間.
		//公式:等加速度直線運動.
		m_vPosition.y += InitialSpeed * Framerate + 0.5f * GravitySpeed * Framerate * Framerate;

		//速度変化(初期速度 += 重力加速度 / 2 * 1フレーム)※/ 2 は落下速度の調整用.
		InitialSpeed += GravitySpeed / 2 * Framerate;
	}
	else
	{
		//重力無し.
		//地面についた.
		m_vPosition.y = 0.2f;
		InitialSpeed = 0.f;
	}
}

void CItemBox::SetItemInfo(CItemType item)
{
	m_ItemType = item;
}

void CItemBox::ShieldEffect()
{
	m_Item.m_ShieldFlag = true;
}

void CItemBox::SpeedUpEffect()
{
	//定数宣言.
	static constexpr float PLUS_SPEED = 0.2f;

	//増加する速度設定.
	m_Item.m_Speed = PLUS_SPEED;
}

void CItemBox::PowerUpEffect()
{
	//定数宣言.
	static constexpr float PLUS_POWER = 3.f;

	//攻撃力を設定.
	m_Item.m_Power = PLUS_POWER;
}

void CItemBox::BlastUpEffect()
{
	//定数宣言.
	static constexpr float BLAST_RAD = 30.f;

	//半径を設定.
	m_Item.m_Blast = BLAST_RAD;
}

void CItemBox::ReflectionEffect()
{
	//反射する.
	m_Item.m_Reflection = true;
}

void CItemBox::ReloadEffect()
{
	//定数宣言.
	static constexpr float RELOAD_INTERVAL = 0.5f;

	//リロード短縮.
	m_Item.m_Reload = RELOAD_INTERVAL;
}

void CItemBox::HitPlayer()
{
	// 消える
	m_Active = false;

	//エフェクトの終了.
	CEffect::Stop(hEffect);

	switch (m_ItemType)
	{
	case CItemType::Shield:
		//一回無敵.
		ShieldEffect();
		break;
	case CItemType::SpeedUp:
		//速度上昇.
		SpeedUpEffect();
		break;
	case CItemType::PowerUp:
		//攻撃力上昇.
		PowerUpEffect();
		break;
	case CItemType::BlastUp:
		//爆風範囲増加.
		BlastUpEffect();
		break;
	case CItemType::Reflection:
		//反射.
		ReflectionEffect();
		break;
	case CItemType::Reload:
		//リロード短縮.
		ReloadEffect();
		break;
	default:
		break;
	}
}

//エフェクトの始まり終わり.
void CItemBox::StartEffect()
{
	//エフェクトのはじめ.
	ItemFlag = true;
}