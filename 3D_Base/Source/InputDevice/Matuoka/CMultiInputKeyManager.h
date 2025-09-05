#pragma once
#include <iostream>
#include <unordered_map>			//unordered_map:連想コンテナ※高速なアクセス.

#include "CInputKeyManager.h"		//キーマネージャー.


//--------------------------------------------------------------
// キー入力専用クラス.
//--------------------------------------------------------------
class CMultiInputKeyManager
{
public:
	CMultiInputKeyManager();
	~CMultiInputKeyManager();

	//常に回って欲しい.
	void Update();
	//初期化.
	void Init();


	//必要なキー分設定してあげる.
	void SetKey(const std::vector<char>& keys);
	//押された瞬間.
	bool NowInputKey(char key);
	//押されている間.
	bool InputKey(char key);
	//離した瞬間.
	bool ReleaseInputKey(char key);



public:
	//必要なキー分を確保する変数コンテナ.
	std::unordered_map<char, CInputKeyManager> m_Key;


};