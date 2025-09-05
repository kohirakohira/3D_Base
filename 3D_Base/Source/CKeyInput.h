#pragma once

#include <Windows.h>
#include <vector>

//==========================================
// キー入力クラス
// 担当：濵口　聖弥
//==========================================

//-----------------------
// 定数宣言
//-----------------------
constexpr int MaxFrame = 10; // 入力時間保持

class CKeyInput
{
public:
	CKeyInput();	// コンストラクタ
	~CKeyInput();	// デストラクタ

	// キー入力の受付
	void Update();

	bool IsDown(int Key);	// キーを押した瞬間(トリガー)
	bool IsRepeat(int Key);	// キーが押され続けている
	bool IsUp(int Key);		// キーが離された(リリース)

private:
	// キーの状態を取得
	bool IsGetKeyInfo(int WantKey, std::vector<int> CheckKey);

private:
	std::vector<std::vector<int>>	m_KeyCnt; // キー入力
	std::vector<int>				m_NowKey; // 今回の入力
	std::vector<int>				m_OldKey; // 1F前のキー入力
};
