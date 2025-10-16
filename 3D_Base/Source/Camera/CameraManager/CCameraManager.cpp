#include "CCameraManager.h"

CCameraManager::CCameraManager()
	: m_Cameras				()
{
}

CCameraManager::~CCameraManager()
{
}

//動き関数.
void CCameraManager::Update()
{
	//カメラの数だけ更新.
	for (auto cam : m_Cameras)
	{
		cam->Update();
	}
}
//描画関数.
void CCameraManager::Draw()
{
	//カメラの数だけ描画.
	for (auto cam : m_Cameras)
	{
		cam->Draw();
	}
}

//初期化関数.
void CCameraManager::Init()
{
	//カメラの数だけ初期化.
	for (auto cam : m_Cameras)
	{
		cam->Init();
	}
}

//インスタンス生成.
void CCameraManager::Create()
{
	//カメラの数だけ生成※m_Cameras.size()分のサイズ.
	for (size_t player = 0; player < m_Cameras.size(); player++)
	{
		//カメラクラスを生成.
		m_Cameras[player] = std::make_shared<CCamera>();
	}
}

//カメラ関数.
void CCameraManager::SetCamera()
{
	//カメラの数だけ設定.
	for (auto cam : m_Cameras)
	{
		cam->SetCamera();
	}
}

//カメラの位置設定.
void CCameraManager::SetCameraPos(const float x, const float y, const float z)
{
	//カメラの数だけ位置設定.
	for (auto cam : m_Cameras)
	{
		cam->SetCameraPos(x, y, z);
	}
}

//ライトの位置設定.
void CCameraManager::SetLightPos(const float x, const float y, const float z)
{
	//カメラの数だけライトの位置設定.
	for (auto cam : m_Cameras)
	{
		cam->SetLightPos(x, y, z);
	}
}

//ライトの回転設定.
void CCameraManager::SetLightRot(const float x, const float y, const float z)
{
	//カメラの数だけライトの回転設定.
	for (auto cam : m_Cameras)
	{
		cam->SetLightRot(x, y, z);
	}
}

//プロジェクション関数.
void CCameraManager::Projection()
{
	//カメラの数だけプロジェクション設定.
	for (auto cam : m_Cameras)
	{
		cam->Projection();
	}
}

//三人称カメラ
void CCameraManager::ThirdPersonCamera(
	CAMERA* pCamera, const D3DXVECTOR3& TargetPos, const float TargetRotY)
{
	//カメラの数だけ三人称カメラ設定.
	for (auto cam : m_Cameras)
	{
		cam->ThirdPersonCamera(pCamera, TargetPos, TargetRotY);
	}
}


//ターゲット位置と回転の設定
void CCameraManager::SetTargetPos(const D3DXVECTOR3& pos)
{
	//カメラの数だけターゲット位置設定.
	for (auto cam : m_Cameras)
	{
		cam->SetTargetPos(pos);
	}
}

void CCameraManager::SetTargetRotY(const float rotY)
{
	for (auto cam : m_Cameras)
	{
		cam->SetTargetRotY(rotY);
	}
}

