#pragma once
#include <iostream>
#include <windows.h>

#include "Global.h"

#include "CDirectX9.h"
#include "CDirectX11.h"

#include "CSceneBase.h"
#include "CUIObject.h"
#include "CChoiceImage.h"

//キーの入力.
#include "CMultiInputKeyManager.h"

//演出用.
#include "CSprite3D.h"
#include "CStaticMesh.h"

#include "CCamera.h"

//地面クラス.
#include "CGround.h"




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