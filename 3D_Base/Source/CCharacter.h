#pragma once
#include <iostream>

#include "CStaticMeshObject.h" // スタティックメッシュオブジェクトを継承
#include <memory>

/**************************************************
*   キャラクタークラス
**/

// 前方宣言
class CInputManager;		// 入力受付クラス.

class CCharacter
	: public CStaticMeshObject
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

protected:
	bool	m_Shot;		//弾を飛ばすフラグ

	std::shared_ptr<RAY>	m_pRayY;	//Y方向へ伸ばしたレイ.

	std::shared_ptr<CInputManager> m_Input;	// 入力受付クラス.
};