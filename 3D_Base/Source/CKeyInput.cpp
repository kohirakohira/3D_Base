#include "CKeyInput.h"

// コンストラクタ
CKeyInput::CKeyInput()
	: m_KeyCnt()
	, m_NowKey()
	, m_OldKey()
{
}

// デストラクタ
CKeyInput::~CKeyInput()
{
}

// キー入力の受付
void CKeyInput::Update()
{
	std::vector<int> m_Key; // 今フレームのキー入力を一時保存

	m_OldKey.clear();

	// 1F前のキーに現在のキー入力を記録
	if (!m_NowKey.empty())
	{
		m_OldKey = m_NowKey;
	}

	// 今フレームのキー状態を初期化
	m_NowKey.clear();

	// 現在押されているキーをチェックして追加
	// キーボードのA～Z
	for (int KeyInput = 'A'; KeyInput <= 'Z'; ++KeyInput)
	{
		if (GetAsyncKeyState(KeyInput) & 0x8000)
		{
			m_Key.emplace_back(KeyInput);
			m_NowKey.emplace_back(KeyInput);
		}
	}

	// その他キー（矢印やスペースなど）
	if (GetAsyncKeyState(VK_SPACE)	& 0x8000)  m_NowKey.emplace_back(VK_SPACE);
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)  m_NowKey.emplace_back(VK_RETURN);
	if (GetAsyncKeyState(VK_LEFT)	& 0x8000)  m_NowKey.emplace_back(VK_LEFT);
	if (GetAsyncKeyState(VK_RIGHT)	& 0x8000)  m_NowKey.emplace_back(VK_RIGHT);
	if (GetAsyncKeyState(VK_UP)		& 0x8000)  m_NowKey.emplace_back(VK_UP);
	if (GetAsyncKeyState(VK_DOWN)	& 0x8000)  m_NowKey.emplace_back(VK_DOWN);

	// 今フレームのキーを履歴に追加
	m_KeyCnt.emplace_back(m_Key);

	// 履歴が多すぎたら削除（最大10フレーム保持）

	if (m_KeyCnt.size() > MaxFrame)
	{
		m_KeyCnt.erase(m_KeyCnt.begin());
	}
}

// キーを押した瞬間(トリガー)
bool CKeyInput::IsDown(int Key)
{
	if (IsGetKeyInfo(Key, m_NowKey)== true // 今押された
		&& IsGetKeyInfo(Key, m_OldKey) == false) // 前フレームは押されていない
	{
		return true;
	}
	return false;
}

// キーが押され続けている
bool CKeyInput::IsRepeat(int Key)
{
	if (IsGetKeyInfo(Key, m_NowKey) == true // 今押された
		&& IsGetKeyInfo(Key, m_OldKey) == true) // 前フレームも押されている
	{
		return true;
	}
	return false;
}

// キーが離された(リリース)
bool CKeyInput::IsUp(int Key)
{
	if (IsGetKeyInfo(Key, m_NowKey) == false // 今押されていない
		&& IsGetKeyInfo(Key, m_OldKey) == true) // 前フレームは押されている
	{
		return true;
	}
	return false;
}

// キーの状態を取得
bool CKeyInput::IsGetKeyInfo(int WantKey, std::vector<int> CheckKey)
{
	for (auto Key : CheckKey)
	{
		if (Key == WantKey)
		{
			return true;
		}
	}
	return false;
}
