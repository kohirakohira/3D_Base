#pragma once
#include "CCharacter.h" // キャラクタークラス継承
#include "CCamera.h"

#include "CInputManager.h" // 入力受付クラス

/**************************************************
*	砲塔クラス.
**/
class CCannon
	: public CCharacter	//キャラクタークラスを継承.
{
public:
	CCannon(int inputID);
	virtual ~CCannon() override;

	virtual void Update() override;
	virtual void Draw(
		D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	void SetCannonPosition(const D3DXVECTOR3& Pos);

	D3DXVECTOR3 GetCannonPosition() const { return m_vPosition; }

	// 入力クラスを設定
	void SetInputManager(const std::shared_ptr<CInputManager>& input);

private:
	// キー入力受付
	void KeyInput();

protected:
	float		m_TurnSpeed;	// 回転速度

	int			m_ShotCoolTime; // ショットのクールタイム
	const int	m_ShotInterval; // ショットのインターバル
	CCamera*	m_pCamera;	

private:
	std::shared_ptr<CInputManager> m_pInput;
};