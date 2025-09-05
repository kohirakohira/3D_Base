#include "CItemBox.h"

CItemBox::CItemBox()
	: InitialSpeed			( 0.f )
	, GravitySpeed			( -9.8f )
	, m_ItemType			()
{
	//大体0.016辺りになる.
	Framerate = 1.f / 60.f;
}

CItemBox::~CItemBox()
{

}

void CItemBox::Update()
{
	//アイテムを落下させるための関数.
	GravityMath();

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

void CItemBox::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	CStaticMeshObject::Draw(View, Proj, Light, Camera);
}

//重力の計算用(下に落下).
void CItemBox::GravityMath()
{
	//位置変化(Y座標 += 初期速度 * 1フレーム + 0.5f * 重力加速度 * 1フレーム * 1フレーム)※0.5fは1/2のこと.
	//「0.5f * 重力加速度 * 1フレーム * 1フレーム」は「加速度によって物体がどれだけ移動するか」を表す式.
	//1フレーム * 1フレームは1フレームにかかる時間.
	//公式:等加速度直線運動.
	m_vPosition.y += InitialSpeed * Framerate + 0.5f * GravitySpeed * Framerate * Framerate;

	//速度変化(初期速度 += 重力加速度 / 2 * 1フレーム)※/ 2 は落下速度の調整用.
	InitialSpeed += GravitySpeed / 2 * Framerate;

	if (m_vPosition.y < 0)
	{
		m_vPosition.y = 20.f;
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
	static constexpr float PLUS_SPEED = 5.f;

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
