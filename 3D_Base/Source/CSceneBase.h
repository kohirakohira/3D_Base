#pragma once
#include <iostream>
#include "CSceneType.h"

#include "CDirectX9.h"
#include "CDirectX11.h"

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

