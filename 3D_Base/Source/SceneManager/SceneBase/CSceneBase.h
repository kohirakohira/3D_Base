#pragma once
//-----ライブラリ-----
#include <iostream>

//-----外部クラス-----
#include "Assets//DirectX//DirectX9//CDirectX9.h" // DirectX9クラス
#include "Assets//DirectX//DirectX11//CDirectX11.h" // DirectX11クラス

#include "SceneManager//SceneType//CSceneType.h" // シーンタイプ
class CSceneBase
{
public:
	CSceneBase() {};
	virtual~CSceneBase() {};

	//動作関数.
	virtual void Update()	= 0;
	//描画関数.
	virtual void Draw()		= 0;
	//初期化関数.
	virtual void Init()		= 0;
	//解放関数.
	virtual void Destroy()	= 0;
	//インスタンス作成関数.
	virtual void Create()	= 0;
	//データの読み込み.
	virtual HRESULT LoadData() = 0;

	//なんのシーンなのかを判定する関数.
	virtual CSceneType GetSceneType() const = 0;
}; 

