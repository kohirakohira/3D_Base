#pragma once

enum class CSceneType
{
	Title,			//タイトル.
	Setting,		//ゲーム設定.
	Main,			//ゲームメイン.
	Result,			//リザルト.
	ResultWin,		//リザルト(一人勝ち).
	ResultDraw,		//リザルト(引き分け).
	Debug,			//デバッグ用のシーン.

	None = -1		//来ないはず.
};
