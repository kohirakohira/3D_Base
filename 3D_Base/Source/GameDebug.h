#pragma once
#include <iostream>
#include <windows.h>

#include "Global.h"

#include "CDirectX9.h"
#include "CDirectX11.h"

#include "CSceneBase.h"
#include "CUIObject.h"


//演出用.
#include "CSprite3D.h"
#include "CStaticMesh.h"

#include "CCamera.h"

#include "CGround.h"




//検証用.
#include "CPlayerManager.h"

//アイテム.
#include "CItemBoxManager.h"


//時間用.
#include "CTimer.h"
//フォント用(仮).
#include "CDebugText.h"



//前方宣言.
class CSprite2D;

class CGameDebug
	:public CSceneBase
{
public:
	CGameDebug(HWND hWnd);
	~CGameDebug()override;


	//動作関数.
	void Update()override;
	//描画関数.
	void Draw()override;
	//初期化関数.
	void Init()override;
	//解放関数.
	void Destroy()override;
	//インスタンス作成関数.
	void Create()override;
	//データの読み込み.
	HRESULT LoadData()override;

	//シーンの種類.
	CSceneType GetSceneType() const override { return m_SceneType; }
public:
	//ウィンドウハンドル.
	HWND		m_hWnd;

	//デバッグ画像.
	std::shared_ptr<CSprite2D>			m_pSpriteDebugIcon;
	//タイマー画像.
	std::shared_ptr<CSprite2D>			m_pSpriteTimer;

	//スタティックメッシュオブジェクトクラス.
	std::shared_ptr<CUIObject>			m_pSpriteObj;
	//スタティックメッシュオブジェクトクラス(タイマー).
	std::shared_ptr<CUIObject>			m_pSpriteTimerObj;

	//スタティックメッシュ(使いまわす資源)
	std::shared_ptr<CStaticMesh>		m_pStaticMeshGround;		//地面

	//シーンタイプ変数.
	CSceneType m_SceneType;


	//演出.
	//地面.
	std::shared_ptr<CGround>			m_pGround;
	//カメラ.
	std::shared_ptr<CCamera>			m_pCamera;

	//描画フラグ.
	bool DrawFlag;





//-----------------------------------------------------------
//--------------------------検証用.--------------------------
//-----------------------------------------------------------
	//スタティックメッシュ(アイテムの箱).
	std::shared_ptr<CStaticMesh>		m_pStaticMeshItemBox;	//アイテムボックス.

	//アイテムクラス.
	std::shared_ptr<CItemBoxManager>	m_pItemBoxManager;


	//戦車のメッシュ(赤と青)
	std::shared_ptr<CStaticMesh>		m_pStaticMesh_TankBodyRed;
	std::shared_ptr<CStaticMesh>		m_pStaticMesh_TankCannonRed;
	
	std::shared_ptr<CStaticMesh>		m_pStaticMesh_TankBodyBlue;
	std::shared_ptr<CStaticMesh>		m_pStaticMesh_TankCannonBlue;

	//プレイヤーマネージャー.
	//std::shared_ptr<CPlayerManager> 	m_pPlayerManager;


	//タイマークラス.
	std::shared_ptr<CTimer>				m_pTimer;
	//デバッグテキスト.
	std::shared_ptr<CDebugText>			m_pDbgText;




private:


};