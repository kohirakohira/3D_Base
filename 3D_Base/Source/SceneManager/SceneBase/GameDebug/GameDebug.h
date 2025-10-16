#pragma once
//-----継承するクラス-----
#include "SceneManager//SceneBase//CSceneBase.h" // シーン基底クラス

//-----ライブラリ-----
#include <iostream>
#include <windows.h>

//-----外部クラス-----
#include "Global.h"

#include "Assets//DirectX//DirectX9//CDirectX9.h"	// DirectX9クラス
#include "Assets//DirectX//DirectX11//CDirectX11.h" // DirectX11クラス

//演出用.
#include "Assets//Sprite//Sprite3D//CSprite3D.h" // 3Dスプライトクラス.
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // スタティックメッシュクラス
#include "GameObject//StaticMeshObject//Ground//CGround.h" // 地面クラス
//検証用.
#include "GameObject//StaticMeshObject//Character//Player//PlayerManager//CPlayerManager.h" // プレイヤーマネージャークラス

//アイテム.
#include "GameObject//StaticMeshObject//ItemBoxManager//CItemBoxManager.h" // アイテムボックスマネージャークラス

#include "Camera//CCamera.h" //カメラクラス
//--------------------------------
// UI
//-------------------------------
#include "GameObject//UI//CUIObject//CUIObject.h" // UIオブジェクトクラス
#include "GameObject//UI//Timer//CTimer.h"		  // タイマークラス

//フォント用(仮).
//-----デバックテキスト-----
#include "Assets//DebugText//CDebugText.h"		// デバッグテキストクラス



//前方宣言.
class CSprite2D;

class CGameDebug
	: public CSceneBase
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