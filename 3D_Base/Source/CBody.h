#pragma once
#include "CCharacter.h" // キャラクタークラスを継承.

#include "CCannon.h" // 戦車：砲塔クラス
#include "CInputManager.h" // 入力受付クラス

/**************************************************
*	車体クラス.
**/

// 前方宣言
class CInputManager;

class CBody
	: public CCharacter	//キャラクタークラスを継承.
{
public:
	// 移動状態
	enum enMoveState
	{
		Stop,		// 停止
		Forward,	// 前進
		Backward,	// 後退
		TurnLeft,	// 左回転
		TurnRight,	// 右回転
	};
public:
	CBody(int inputID);
	virtual ~CBody() override;

	virtual void Update() override;
	virtual void Draw(
		D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	// ラジコン操作
	void RadioControl();

	// 入力クラスを設定
	void SetInputManager(const std::shared_ptr<CInputManager>& input);

private:
	// キー入力受付.
	void KeyInput();

protected:
	float		m_TurnSpeed;	// 回転速度
	float		m_MoveSpeed;	// 移動速度
	enMoveState m_MoveState;	// 移動状態

private:
	std::shared_ptr<CInputManager> m_pInput;
};
