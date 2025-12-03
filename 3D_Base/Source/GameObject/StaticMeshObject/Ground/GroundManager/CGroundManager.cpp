#include "CGroundManager.h"

CGroundManager::CGroundManager()
	: m_Ground				()
{
}

CGroundManager::~CGroundManager()
{
}

//更新関数.
void CGroundManager::Update()
{
	m_Ground[0]->Move(30.0f);
	auto pos = m_Ground[0]->GetPosition().z;
	if (pos < -200.0f)
	{
		m_Ground[1]->Move(30.0f);
	}
}

//描画関数.
void CGroundManager::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	for (auto& ground : m_Ground)
	{
		ground->Draw(View, Proj, Light, Camera);
	}
}

//インスタンス生成.
void CGroundManager::Create()
{
	for (int No = 0; No < 2; No++)
	{
		m_Ground.push_back(std::make_shared<CGround>());
	}
}

//初期化.
void CGroundManager::Init()
{
	for (auto& ground : m_Ground)
	{
		ground->Init();
	}
}

//メッシュのアタッチ.
void CGroundManager::AttachMesh(std::shared_ptr<CStaticMesh> mesh)
{
	for (auto& ground : m_Ground)
	{
		ground->AttachMesh(mesh);
	}
}

//位置設定.
void CGroundManager::SetPosition(float x, float y, float z)
{
	for (auto& ground : m_Ground)
	{
		ground->SetPosition(x, y, z);
	}
}

//回転設定.
void CGroundManager::SetRotato(float x, float y, float z)
{
	for (auto& ground : m_Ground)
	{
		ground->SetRotation(x, y, z);
	}
}