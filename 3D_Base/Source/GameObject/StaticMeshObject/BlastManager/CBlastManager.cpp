#include "CBlastManager.h"

CBlastManager::CBlastManager()
	: m_Blast			( )
	, m_NormalRadius	( 4.0f )
	, m_MaxRadius		( 10.0f )
{
}

CBlastManager::~CBlastManager()
{
}

//インスタンス生成.
void CBlastManager::Create(const D3DXVECTOR3& pos, std::shared_ptr<CStaticMesh> mesh, float s)
{
	//インスタンス生成.
	auto blast = std::make_shared<CBlast>();

	//メッシュの設定.
	blast->AttachMesh(mesh);
	//秒数の設定.
	blast->SetSpeed(s);
	//半径の設定.
	blast->SetMaxRadius(m_NormalRadius);

	//初期位置.
	blast->SetPosition(pos);
	//爆発開始フラグ.
	blast->SetBomStart(true);

	//インスタンスを移動.
	m_Blast.push_back(blast);
}

//更新処理.
void CBlastManager::Update()
{
	for (auto& blast : m_Blast)
	{
		blast->Update();
	}

	//std::remove_if(begin, end, pred)：削除対象でない要素だけを前に詰める.
	//[](auto& b) {return b->IsFinish(); }：ラムダ式※爆風オブジェクトに対してIsBomFinish()がtrueかどうかを返すだけの関数.
	//										ラムダ式で出た結果、終了していたら配列から消す.
	m_Blast.erase(
		std::remove_if(m_Blast.begin(), m_Blast.end(),
			[](auto& blast) {return blast->IsBomFinish(); }),m_Blast.end()
	);
}

//描画処理.
void CBlastManager::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	for (auto& blast : m_Blast)
	{
		blast->Draw(View, Proj, Light, Camera);
	}
}

//爆風一つを取得.
std::shared_ptr<CBlast> CBlastManager::GetBlast(int index)
{
	if (index >= 0 && index < m_Blast.size())
	{
		return m_Blast[index];
	}
}
