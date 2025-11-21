#pragma once
//-----継承するクラス-----
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" // スタティックメッシュオブジェクトクラス

//-----ライブラリ-----
#include <iostream>
#include <memory>

/**************************************************
*   キャラクタークラス
**/

// 前方宣言
class CInputManager;		// 入力受付クラス.

//struct TankTuning
//{
//	float moveSpeed = 0.08f;		//前進、後退
//	float bodyTurnSpeed = 0.03f;	//車体ヨー
//	float turretTurnSpeed = 0.03f;	//砲塔ヨー
//	float cannonHeight = 0.3f;		//砲塔の取り付けの高さ
//};

class CCharacter
	: public CStaticMeshObject // スタティックメッシュオブジェクトクラスを継承.
{
public:
	CCharacter();
	virtual ~CCharacter();

	virtual void Update() override;
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	//Y軸方向へ伸ばしたレイを取得.
	std::shared_ptr<RAY> GetRayY() const { return m_pRayY; }

	//使っていないみたいなので消した
	////初期値設定用関数.
	//void SettingTune();

	// パラメータの設定
	virtual void SetTuning(const TankTuning& tuning) = 0;
	// パラメータの取得
	virtual const TankTuning& GetTuning() const = 0;

protected:
	std::shared_ptr<RAY>	m_pRayY;	//Y方向へ伸ばしたレイ.

	TankTuning m_Tuning;	//戦車の情報
	
};