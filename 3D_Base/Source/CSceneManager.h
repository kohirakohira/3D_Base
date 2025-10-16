#pragma once
#include <iostream>
#include <windows.h>

#include "CSceneBase.h"		//基底クラス.
#include "CGameMain.h"		//ゲームメインクラス.
#include "CGameTitle.h"		//タイトル.
#include "CGameResult.h"	//リザルトクラス(勝ち・引き分け両立).
#include "CGameSettings.h"	//ゲーム設定クラス.

#include "GameDebug.h"		//デバッグ用クラス.

class CSceneManager
{
public:
	CSceneManager(HWND hWnd );
	~CSceneManager();

	//動作関数.
	void Update();
	//描画関数.
	void Draw();

	//読み込み.
	HRESULT LoadData();

	//インスタンス生成.
	void Create();

	//シーン遷移関数.
	void Change(std::shared_ptr<CSceneBase> InScene);
	
	
public:
	//確認用の変数.
	bool			m_Flag;

	//ウィンドウハンドル.
	HWND			m_hWnd;

private:
	//シーンを保存するようの変数.
	std::shared_ptr<CSceneBase> m_pScene;

	//シーンを保存しておくモノ.
	CSceneType					m_CurrentSceneType;
	//次のシーン.
	CSceneType					m_NextType;

};