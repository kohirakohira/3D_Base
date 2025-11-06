#pragma once
//STL.
#include <iostream>
#include <memory>

//Xinputクラス.
#include "InputDevice\Input\XInput\CXInput.h"

//=====================================================
// コントローラークラス.
//=====================================================
class CController
{
public:
	// スティックの入力方向
	enum class Direction
	{
		Up,         // 上
		Down,       // 下
		Left,       // 左
		Right,      // 右

		UpLeft,     // 左上
		UpRight,    // 右上
		DownLeft,   // 左下
		DownRight,  // 右下

		None = -1   // 無設定
	};

public:
	//explicit：コンパイラの勝手な変換を防ぐ保険.
	explicit CController(int index);
	~CController();

	//動作関数.
	void Update();

	//接続判定.
	bool CheckConnected() const;

	//切断処理.
	void ControllerAmputation();

	//ラッパー関数.
	//関数名(・・・, bool just = false)：このbool型の引数はデフォルト引数.
	//例：関数名(・・・)：この書き方だとfalse.
	//例：関数名(・・・, true)：この書き方だとtrue.
	bool Down(CXInput::KEY key, bool just = false) const ;
	bool Up(CXInput::KEY key) const ;
	bool Repeat(CXInput::KEY key) const ;

	//スティックの方向を反映させる※左.
	Direction GetLeftStickDirection(float value);
	//スティックの方向を反映させる※右.
	Direction GetRightStickDirection(float value);

	//XY座標から方向判定する関数.
	Direction GetDirectionFromXY(float x, float y, float value);

	//スティック値を-1.0f～1.0fに正規化して返す.
	float GetLeftStickX() const;
	float GetLeftStickY() const;
	float GetRightStickX() const;
	float GetRightStickY() const;

	//トリガー値(0～255)を0.0～1.0にする.
	float GetLeftTrigger() const;
	float GetRightTrigger() const;

	//振動.
	void SetVibration(WORD left, WORD right);

	//インデックス(0～3).
	int Getindex() const { return m_Index; }

private:
	//メンバ変数.
	std::unique_ptr<CXInput> m_Pad;	//実際の入力処理.
	int		m_Index;				//コントローラーのID.
	bool	m_Connected;			//接続フラグ.
};