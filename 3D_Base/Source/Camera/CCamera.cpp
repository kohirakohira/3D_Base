#include "CCamera.h"
#include <d3dx9math.h>	//カメラの座標計算用.
#include <d3d9.h>		//描画する用.
#include <string>

CCamera::CCamera()
	: m_TargetPos			(0.f, 0.f, 0.f)
	, m_TargetRotY			(0.f)
	, m_Distance			(4.f)
	, m_HeightOffset		(2.0f)
	, m_Up					(0.0f, 1.0f, 0.0f)
{
	//カメラ座標.
	m_Camera.vPosition = D3DXVECTOR3(0.0f, 2.0f, 0.0f);
	m_Camera.vLook = D3DXVECTOR3(0.0f, 2.0f, 5.0f);

	//ライト情報
	m_Light.Position = D3DXVECTOR3(0.f, 3.f, 0.f);	//ライト方向
	m_Light.Range = 100.0f;
	m_Light.Color = D3DXVECTOR3(1.f, 1.f, 1.f);
	m_Light.fIntensity = 1.0f;
	m_Light.Atten = D3DXVECTOR3(1.0f, 0.f, 0.05f);

	m_Mode = CameraMode::ThirdPerson;
}

//Updateではなく授業コードの三人称カメラをベースにする
void CCamera::Update()
{
	//各モードの処理.
	switch (m_Mode)
	{
	case CCamera::CameraMode::ThirdPerson:
		UpdateThirdPerson();
		break;
	case CCamera::CameraMode::Free:
		UpdateFree();
		break;
	default:
		break;
	}
	//ビュー・プロジェクション行列の更新.
	ViewMatrixUpdate();
	ProjMatrixUpdate();

}

void CCamera::Init()
{

	//カメラの初期化.
	m_Camera.vPosition	= D3DXVECTOR3(0.0f, 2.0f, -5.0f);
	m_Camera.vLook		= D3DXVECTOR3(0.0f, 2.0f, 0.0f);

	m_Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	ProjMatrixUpdate();

}

void CCamera::SetCameraPos(float x, float y, float z)
{
	//位置の設定.
	m_Camera.vPosition = { x, y, z };
}


void CCamera::SetLightRot(float x, float y, float z)
{
	m_Light.vDirection = { x, y, z };
}

void CCamera::SetTargetPos(const D3DXVECTOR3& pos)
{
	m_TargetPos = pos;
}

void CCamera::SetTargetRotY(float rotY)
{
	m_TargetRotY = rotY;
}

//カメラのモード切り替え.
void CCamera::SetCameraMode(CameraMode mode)
{
	m_Mode = mode;
}

//三人称視点.
void CCamera::UpdateThirdPerson()
{
	//方向(Z軸).
	D3DXVECTOR3 backward(0.0f, 0.0f, 1.0f);
	//回転.
	D3DXMATRIX rotY;
	//Y回転行列.
	D3DXMatrixRotationY(&rotY, m_TargetRotY);

	//ベクトルに行列を掛ける.
	D3DXVec3TransformCoord(&backward, &backward, &rotY);

	//向きベクトル x 距離 = オフセット.
	m_Camera.vPosition = m_TargetPos - backward * m_Distance;
	m_Camera.vPosition.y += m_HeightOffset;

	//見る位置 = キャラ位置 + 向き x 少し.
	m_Camera.vLook = m_TargetPos + backward * 3.0f;
	m_Camera.vLook.y += 0.5f;

}

//自由視点.
void CCamera::UpdateFree()
{
	float add_value = 0.05f;

	if (GetAsyncKeyState('G') & 0x8000)
	{
		m_Camera.vPosition.y += add_value;
	}
	if (GetAsyncKeyState('T') & 0x8000)
	{
		m_Camera.vPosition.y -= add_value;
	}
	if (GetAsyncKeyState('H') & 0x8000)
	{
		m_Camera.vPosition.x -= add_value;
	}
	if (GetAsyncKeyState('F') & 0x8000)
	{
		m_Camera.vPosition.x += add_value;
	}
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		m_Camera.vPosition.z += add_value;
	}
	if (GetAsyncKeyState('Y') & 0x8000)
	{
		m_Camera.vPosition.z -= add_value;
	}
}

//ビュー行列の更新.
void CCamera::ViewMatrixUpdate()
{
	D3DXMatrixLookAtLH(
		&m_mView,				//ビュー行列.
		&m_Camera.vPosition,	//カメラがどこにあるか.
		&m_Camera.vLook,		//カメラがどこを見ているか.
		&m_Up					//どっちが上か.
	);
}

//プロジェクション行列の更新.
void CCamera::ProjMatrixUpdate()
{
	//視野角.
	float fovY		= D3DXToRadian(45.0f);
	//アスペクト比.
	float aspect	= static_cast<float>(WND_W) / static_cast<float>(WND_H);
	//描画する距離(スタート位置).
	float nearZ		= 0.3f;
	//描画する距離(エンド位置).
	float farZ		= 2000.0f;

	D3DXMatrixPerspectiveFovLH(
		&m_mProj,	//(out)プロジェクション計算結果.
		fovY,		//視野角（FOV：Field of View）.
		aspect,		//アスペクト比.
		nearZ,		//近いビュー平面のz値.
		farZ		//遠いビュー平面のz値.
	);

}
