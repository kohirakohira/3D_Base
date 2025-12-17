#pragma once
#include <d3dx9.h>
#include <iostream>

//-----DirectX.-----
#include "Assets//DirectX//DirectX11//CDirectX11.h" // DirectX11クラス
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/Util/Util.h"	//ユーティリティ

class CCamera
{
public:
	CCamera();
	~CCamera() = default;	//特別な処理はないのでデフォルトにしておく

	void Update();	//アップデートを一旦三人称カメラとして使う
	void Info();
	void Init();
	//カメラ関数.
	void SetCamera();
	//カメラの位置設定.
	void SetCameraPos(float x, float y, float z);
	//ライトの回転設定.
	void SetLightRot(float x, float y, float z);
	//プロジェクション関数.
	void Projection();

	//三人称カメラ
	void ThirdPersonCamera(
		CAMERA* pCamera, const D3DXVECTOR3& TargetPos, float TargetRotY);

	//ビュー行列の取得
	D3DXMATRIX GetViewMatrix() const { return m_mView; }

	//ターゲット位置と回転の設定
	void SetTargetPos(const D3DXVECTOR3& pos);
	void SetTargetRotY(float rotY);

	//位置の取得.
	D3DXVECTOR3& GetCameraPosition() { return m_Camera.vPosition; }
	//注視点の位置取得.
	D3DXVECTOR3& GetLookPosition() { return m_Camera.vLook; }
	//自由移動.
	void FreeMove();

public:
	//境界線を消すのに使用するかも.
	FOGPARAM GetFog() const { return m_Fog; }
	void SetFog();
	void SetFogParam(const FOGPARAM fog) { m_Fog = fog; }
	void SetIsFog(bool f);
public:
	//ライト情報.
	const LIGHT& GetLight() const { return m_Light; }
	void SetLightPos(float x, float y, float z) { m_Light.Position = D3DXVECTOR3(x, y, z); }
	void SetLightColor(float r, float g, float b) { m_Light.Color = D3DXVECTOR3(r, g, b); }
	void SetLightIntensity(float I) { m_Light.fIntensity = I; }
	void SetLightRange(float r) { m_Light.Range = r; }
	void SetLightAtten(float kc, float kl, float kq) { m_Light.Atten = D3DXVECTOR3(kc, kl, kq); }

	const D3DXVECTOR3& GetPosition() const { return m_Position; }
	void SetCameraPosition(D3DXVECTOR3 pos) { m_Position = pos; }
public:
	//構造体設定変数.
	CAMERA			m_Camera;
	LIGHT			m_Light;
	FOGPARAM		m_Fog;
	//行列.
	D3DXMATRIX		m_mView;	//ビュー(カメラ)行列.
	D3DXMATRIX		m_mProj;	//射影（プロジェクション）行列.
private:
	D3DXVECTOR3		m_TargetPos;		//ターゲットの位置
	float			m_TargetRotY;		//ターゲットのY軸回転
	D3DXVECTOR3		m_Position;			//カメラの位置
	float			m_Distance;			//カメラとターゲットの距離
	float			m_HeightOffset;		//ターゲットからの高さ
	D3DXVECTOR3		m_Up;				//上方向
	D3DXVECTOR3		m_LookAt;			//注視点



};