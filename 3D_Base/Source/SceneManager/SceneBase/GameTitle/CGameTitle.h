#pragma once
//-----継承するクラス-----
#include "SceneManager//SceneBase//CSceneBase.h" // シーン基底クラス

//-----ライブラリ-----
#include <iostream>
#include <windows.h>

//-----グローバル-----
#include "Global.h"

//-----外部クラス-----
#include "Assets//DirectX//DirectX9//CDirectX9.h" // DirectX9クラス
#include "Assets//DirectX//DirectX11//CDirectX11.h" // DirectX11クラス

#include "GameObject//UI//CUIObject//CUIObject.h"		// UIオブジェクトクラス
#include "GameObject//UI//ChoiceImage//CChoiceImage.h"  // 選択肢クラス

//キーの入力.
#include "InputDevice//Matuoka//CMultiInputKeyManager.h" // キー入力用

//演出用.
#include "Assets//Sprite//Sprite3D//CSprite3D.h" // 3Dスプライトクラス.

#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // スタティックメッシュクラス
#include "Camera//CCamera.h" //カメラクラス

//地面クラス.
#include "GameObject//StaticMeshObject//Ground//CGround.h" // 地面クラス


//前方宣言.
class CSprite2D;

class CGameTitle
	:public CSceneBase
{
public:
	CGameTitle(HWND hWnd);
	~CGameTitle()override;


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

	//タイトル画像.
	std::shared_ptr<CSprite2D>			m_pSpriteTitle;
	//選択肢画像.
	std::shared_ptr<CSprite2D>			m_SpriteChoice;
	//選択画像(プレイ&エンド).
	std::shared_ptr<CSprite2D>			m_SpritePlaySelect;
	std::shared_ptr<CSprite2D>			m_SpriteEndSelect;

	//タイトル画像.
	std::shared_ptr<CUIObject>			m_pSpriteTitleImg;
	//選択肢画像.
	std::shared_ptr<CChoiceImage>		m_pSpriteChoiceImg;	
	//選択画像(プレイ&エンド).
	std::shared_ptr<CUIObject>			m_pSpritePlaySelectImg;
	std::shared_ptr<CUIObject>			m_pSpriteEndSelectImg;


	//シーンタイプ変数.
	CSceneType m_SceneType;

	//キー入力.
	std::shared_ptr<CMultiInputKeyManager>	m_KeyInput;

	//描画フラグ.
	bool DrawFlag;



//------------------------------------------------------------------------------------------
//	演出.
//------------------------------------------------------------------------------------------
public:
	//カメラクラス.
	std::shared_ptr<CCamera>			m_pCamera;

	//地面クラス.
	std::shared_ptr<CGround>			m_pGround;
	//地面のメッシュ.
	std::shared_ptr<CStaticMesh>		m_StaticMeshGround;

	
	
	float y = 0.f;

	float rad = 0.f;



};