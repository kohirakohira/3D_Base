#pragma once
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" /* 継承クラス || スタティックメッシュオブジェクトクラス */

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

	//弾を飛ばしたいか確認
	bool IsShot() const { return m_Shot; }

	//Y軸方向へ伸ばしたレイを取得.
	std::shared_ptr<RAY> GetRayY() const { return m_pRayY; }

	// 入力受付を取得.
	std::shared_ptr<CInputManager> GetInput() const
	{
		return m_Input;
	}
	
	void SetInput(const std::shared_ptr<CInputManager> input) { m_Input = input; }

	//パラメータの外部関数
	void SetTuning(const TankTuning& tuning) { m_Tune = tuning; }
	const TankTuning& GetTuning() const { return m_Tune; }

	//初期値設定用関数.
	void SettingTune();



protected:
	TankTuning m_Tune;
	bool	m_Shot;		//弾を飛ばすフラグ

	std::shared_ptr<RAY>	m_pRayY;	//Y方向へ伸ばしたレイ.

	std::shared_ptr<CInputManager> m_Input;	// 入力受付クラス.

};