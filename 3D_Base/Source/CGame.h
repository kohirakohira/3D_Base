#pragma once
#include "CSceneManager.h"

/********************************************************************************
*	ゲームクラス.
**/
class CGame
{
public:
	static constexpr int ENEMY_MAX = 3;	//エネミーの最大数

	CGame(HWND hWnd );
	~CGame();

	void Create();
	HRESULT LoadData();
	void Release();

	void Update();
	void Draw();

private:
	//ウィンドウハンドル.
	HWND		m_hWnd;

	//シーンマネージャー.
	std::shared_ptr<CSceneManager>	m_pSceneManager;

private:
	//=delete「削除定義」と呼ばれる機能.
	//指定された場合、その関数は呼び出せなくなる.
	CGame() = delete;	//デフォルトコンストラクタ禁止.
	CGame( const CGame& ) = delete;
	CGame& operator = (const CGame& rhs ) = delete;
};