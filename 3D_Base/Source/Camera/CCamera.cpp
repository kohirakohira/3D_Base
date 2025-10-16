#include "CCamera.h"
#include <d3dx9math.h>	//カメラの座標計算用.
#include <d3d9.h>		//描画する用.
#include <string>

CCamera::CCamera()
	: m_TargetPos	(0.f, 0.f, 0.f)
	, m_TargetRotY	(0.f)
	, m_Position	(0.f,2.f,0.f)	
	, m_Distance	(4.f)
	, m_HeightOffset(2.0f)
	, m_LookAt(0.0f, 2.0f, 0.0f)
	, m_Up(0.0f, 1.0f, 0.0f)


{
	//カメラ座標.
	m_Camera.vPosition = D3DXVECTOR3(0.0f, 2.0f, 0.0f);
	m_Camera.vLook = D3DXVECTOR3(0.0f, 2.0f, 5.0f);

	//ライト情報
	m_Light.vDirection = D3DXVECTOR3(1.5f, 1.f, -1.f);	//ライト方向

	//Update();
}

//Updateではなく授業コードの三人称カメラをベースにする
void CCamera::Update()
{
	//回転からZ方向のオフセットを求める
	D3DXVECTOR3 backward(0.f, 0.f, 1.f);
	D3DXMATRIX rotY;
	D3DXMatrixRotationY(&rotY, m_TargetRotY);
	//ターゲットから見たカメラの位置
	D3DXVec3TransformCoord(&backward, &backward, &rotY);

	//プレイヤーの位置からｍ距離分だけ下がった場所にカメラをおく
	m_Position = m_TargetPos - (backward * m_Distance);
	//見下ろし視点にする
	m_Position.y += m_HeightOffset;

	//視線の先.ターゲット位置+少し前を見るようにする
	D3DXVECTOR3 forward = backward * 3.0f;
	m_LookAt = m_TargetPos + forward;
	m_LookAt.y += 0.5f;	//視線の高さ微調整

	//ビュー行列を計算
	D3DXMatrixLookAtLH(&m_mView, &m_Position, &m_LookAt, &m_Up);

	//プロジェクションもここで更新しておく
	float fovY = D3DXToRadian(45.0f);
	float aspect = static_cast<FLOAT>(WND_W) / static_cast<FLOAT>(WND_H);
	D3DXMatrixPerspectiveFovLH(&m_mProj, fovY, aspect, 0.1f, 100.0f);


	//SetCamera.Draw側で使う構造体を追尾結果にあわせて更新
	m_Camera.vPosition = m_Position;
	m_Camera.vLook = m_LookAt;

}


void CCamera::Draw()
{
	//SetCamera();
	//Projection();
}

void CCamera::Init()
{

}

void CCamera::SetCamera()
{
	//ビューの設定.
	D3DXVECTOR3 cam_pos = m_Camera.vPosition;
	D3DXVECTOR3 cam_look = m_Camera.vLook;
	D3DXVECTOR3	vUpVec(0.0f, 1.0f, 0.0f);	//上方（ベクトル）.

	//ビュー（カメラ）変換.
	D3DXMatrixLookAtLH(
		&m_mView,	//(out)ビュー計算結果.
		&cam_pos, &cam_look, &vUpVec);
}

void CCamera::SetCameraPos(float x, float y, float z)
{
	//位置の設定.
	m_Camera.vPosition = { x, y, z };
}

void CCamera::SetLightPos(float x, float y, float z)
{
	//位置の設定.
	m_Light.Position = { x, y, z };	//ライト方向
}

void CCamera::SetLightRot(float x, float y, float z)
{
	m_Light.vDirection = { x, y, z };
}

void CCamera::Projection()
{
	//y方向の視野角。数値を大きくしたら視野が狭くなる.
	float fov_y = static_cast<FLOAT>(D3DXToRadian(45.0));	//ラジアン値.
	//アスペクト（幅÷高さ）.
	float aspect = static_cast<FLOAT>(WND_W) / static_cast<FLOAT>(WND_H);
	float near_z = 0.3f;
	float far_z = 300.0f;

	//プロジェクション（射影）変換.
	D3DXMatrixPerspectiveFovLH(
		&m_mProj,	//(out)プロジェクション計算結果.
		fov_y,		//視野角（FOV：Field of View）.
		aspect,		//アスペクト.
		near_z,		//近いビュー平面のz値.
		far_z);	//遠いビュー平面のz値.
}

//三人称カメラ
void CCamera::ThirdPersonCamera(
	CAMERA* pCamera, const D3DXVECTOR3& TargetPos, float TargetRotY)
{
	//Z軸ベクトル（Z+方向の単位ベクトル）
	D3DXVECTOR3 vecAxisZ(0.f, 0.f, 1.f);

	//Y方向の回転行列
	D3DXMATRIX mRotationY;
	//Y軸回転行列を作成
	D3DXMatrixRotationY(
		&mRotationY,	//(out)行列
		TargetRotY);	//対象のY方向の回転値

	//Y軸回転行列を使ってZ軸ベクトルを座標変換する
	D3DXVec3TransformCoord(
		&vecAxisZ,		//(out)Z軸ベクトル
		&vecAxisZ,		//(in)Z軸ベクトル
		&mRotationY);	//Y軸回転行列

	//カメラの位置、注視位置を対象にそろえる
	pCamera->vPosition = TargetPos;
	pCamera->vLook = TargetPos;

	//カメラの位置、注視位置をZ軸ベクトルを用いて調整
	pCamera->vPosition -= vecAxisZ * 4.f;	//対象の背中側
	pCamera->vLook += vecAxisZ * 3.f;	//対象を挟んで向こう側

	//カメラの位置、注視位置の高さをそれぞれ微調整
	pCamera->vPosition.y += 2.f;
	pCamera->vLook.y += 0.5f;
}

void CCamera::SetTargetPos(const D3DXVECTOR3& pos)
{
	m_TargetPos = pos;
}

void CCamera::SetTargetRotY(float rotY)
{
	m_TargetRotY = rotY;
}

void CCamera::FreeMove()
{
	//カメラ座標のデバックコマンド.
	float add_value = 0.1f;
	if (GetAsyncKeyState('G') & 0x8000) {
		m_Camera.vPosition.y -= add_value;
	}
	if (GetAsyncKeyState('T') & 0x8000) {
		m_Camera.vPosition.y += add_value;
	}
	if (GetAsyncKeyState('H') & 0x8000) {
		m_Camera.vPosition.x -= add_value;
	}
	if (GetAsyncKeyState('F') & 0x8000) {
		m_Camera.vPosition.x += add_value;
	}
	if (GetAsyncKeyState('Q') & 0x8000) {
		m_Camera.vPosition.z += add_value;
	}
	if (GetAsyncKeyState('E') & 0x8000) {
		m_Camera.vPosition.z -= add_value;
	}
}
