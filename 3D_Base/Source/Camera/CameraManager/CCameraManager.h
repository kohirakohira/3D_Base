#pragma once
//-----ライブラリ-----
#include <iostream>
#include <array>	

//-----子クラス-----
#include "Camera\\CCamera.h"

//カメラマネージャークラス.
class CCameraManager
{
public:
	CCameraManager();
	~CCameraManager();

	//動き関数.
	void Update();
	//描画関数.
	void Draw();
	//初期化関数.
	void Init();
	//インスタンス生成.
	void Create();

	//プロジェクション関数.
	void Projection();

	//三人称カメラ
	void ThirdPersonCamera(
		CAMERA* pCamera, const D3DXVECTOR3& TargetPos, const float TargetRotY);
public:
	//カメラ関数.
	void SetCamera();
	//カメラの位置設定.
	void SetCameraPos(const float x, const float y, const float z);
	//ライトの位置設定.
	void SetLightPos(const float x, const float y, const float z);
	//ライトの回転設定.
	void SetLightRot(const float x, const float y, const float z);
	//ターゲット位置と回転の設定
	void SetTargetPos(const D3DXVECTOR3& pos);
	void SetTargetRotY(const float rotY);

public:
	//カメラ位置の取得.
	D3DXVECTOR3& GetCameraPosition();
	//注視点位置の取得.
	D3DXVECTOR3& GetLookPosition();
	//ビュー行列の取得
	D3DXMATRIX GetViewMatrix() const;
	//カメラのインデックス番号を取得する.
	std::shared_ptr<CCamera> GetCamera(int idx) const
	{
		//インデックス番号が範囲外(0より少ない、又は4以上)ならnullptrを返す.
		if (idx < 0 || idx >= PLAYER_MAX)
		{
			return nullptr;
		}
		return m_Cameras[idx];
	}

public:
	//カメラクラス変数(プレイヤーの数分)※デフォルト初期化：○○{}.
	std::array<std::shared_ptr<CCamera>, PLAYER_MAX>		m_Cameras{};

private:

};