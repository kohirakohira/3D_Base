#pragma once

enum class CItemType
{
	Shield = 0,		//シールド.
	SpeedUp,		//スピードアップ.
	PowerUp,		//パワーアップ.
	BlastUp,		//ブラストアップ(爆風範囲上昇).
	Reflection,		//リフレクション(反射※1回).
	Reload,			//装填時間短縮(リロード短縮).

	None = -1		//来ないはず.
};