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

#include "CPlayerManager.h"
#include "CGround.h"

//キーバインド.
#include "CMultiInputKeyManager.h"

//選択画像.
#include "CSelectIcon.h"
//選択肢.
#include "CChoiceImage.h"


//前方宣言.
class CSprite2D;

class CGameResult
	:public CSceneBase
{
public:
	CGameResult(HWND hWnd);
	~CGameResult()override;


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

	//勝ちか引き分けの判定クラス.
	CSceneType WinOrDrawFunction();

	//シーンの種類.
	CSceneType GetSceneType() const override { return m_SceneType; }

public:
	//ウィンドウハンドル.
	HWND		m_hWnd;

	//タイトル画像.
	std::shared_ptr<CSprite2D>			m_pSpriteResult;
	std::shared_ptr<CSprite2D>			m_pSpriteSelect;
	//選択肢画像.
	std::shared_ptr<CSprite2D>			m_SpriteChoice;

	//スタティックメッシュオブジェクトクラス.
	std::shared_ptr<CUIObject>			m_pSpriteObj;
	//スタティックメッシュ(使いまわす資源)
	std::shared_ptr<CStaticMesh>		m_pStaticMeshFighter;		//自機
	std::shared_ptr<CStaticMesh>		m_pStaticMeshGround;		//地面
	std::shared_ptr<CStaticMesh>		m_pStaticMeshCloud;			//雲

	//シーンタイプ変数.
	CSceneType m_SceneType;


	//演出.
	std::shared_ptr<CPlayerManager>		m_pPlayerManager;
	std::shared_ptr<CGround>			m_pGround;
	//カメラ.
	std::shared_ptr<CCamera>			m_pCamera;

	//選択アイコン.
	std::shared_ptr<CSelectIcon>		m_pSelectIcon;
	//選択肢.
	std::shared_ptr<CChoiceImage>		m_pChoiceIcon;

	//描画フラグ.
	bool DrawFlag;

private:
	//確認用.
	int NoMajic = 0;
	//キーバインド.
	std::shared_ptr<CMultiInputKeyManager>	m_Key;
};