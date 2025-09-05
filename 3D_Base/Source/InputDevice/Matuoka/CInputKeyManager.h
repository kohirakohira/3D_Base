#pragma once
#include <iostream>


//--------------------------------------------------------------
// キー入力専用クラス.
//--------------------------------------------------------------
// NowInputKey()	：押された瞬間を返してくれる.
// InputKey()		：押されている間を返してくれる.
// ReleseInputKey()	：離した瞬間を返してくれる.


class CInputKeyManager
{
public:
	CInputKeyManager();
	~CInputKeyManager();

	//常に回してほしい.
	void Update();
	//初期化.
	void Init();

	//押された瞬間.
	bool NowInputKey();
	//押されている間.
	bool InputKey();
	//離した瞬間.
	bool ReleseInputKey();

	//入力を受け付ける.
	void SetKey(char key) { CheckKey = key; }

private:
	//押し離しの判定.
	bool Input;
	//1フレーム前の情報.
	bool OldInput;
	//押されているかチェックするキー.
	char CheckKey;

};