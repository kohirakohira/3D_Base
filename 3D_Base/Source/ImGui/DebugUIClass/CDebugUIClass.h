#pragma once
//-----STL-----.
#include <iostream>

//.

//========================================================================
//		Imgui専用デバッグUIクラス.
//========================================================================
class CDebugUIClass
{
public:
	CDebugUIClass();
	~CDebugUIClass();

	//更新処理(調整用).
	void Update();

	//描画処理.
	void Draw();

private:

};