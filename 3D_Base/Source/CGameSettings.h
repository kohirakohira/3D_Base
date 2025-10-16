#pragma once
#include <iostream>
#include <windows.h>

#include "Global.h"

#include "CDirectX9.h"
#include "CDirectX11.h"

#include "CSceneBase.h"
#include "CImageObject.h"
#include "CUIObject.h"

#include "CChoiceImage.h"
#include "CSelectIcon.h"

#include "CSprite2D.h"
#include "CStaticMesh.h"

//キー入力用.
#include "CMultiInputKeyManager.h"


//前方宣言.
class CSprite2D;

class CGameSettings
	:public CSceneBase
{
public:
	CGameSettings(HWND hWnd);
	~CGameSettings()override;


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
	//背景を動かす関数.
	void MoveBackGround();

	//シーンの種類.
	CSceneType GetSceneType() const override { return m_SceneType; }
public:
	//ウィンドウハンドル.
	HWND		m_hWnd;

	//タイトル画像.
	std::shared_ptr<CSprite2D>			m_pSpriteSetting;
	//タイトル画像の背景.
	std::shared_ptr<CSprite2D>			m_pSpriteSettingBackGround;
	//「開始する？」画像.
	std::shared_ptr<CSprite2D>			m_pSpriteStart;
	//選択肢画像.
	std::shared_ptr<CSprite2D>			m_SpriteChoice;
	//選択肢画像.
	std::shared_ptr<CSprite2D>			m_SpriteYesSelect;
	std::shared_ptr<CSprite2D>			m_SpriteNoSelect;



	//タイトル表示用板ポリ.
	std::shared_ptr<CImageObject>				m_pSpriteSettingImg;
	//背景模様表示用板ポリ.
	std::shared_ptr<CImageObject>				m_pSpriteSettingBackGroundImg;
	//開始する？表示用板ポリ.
	std::shared_ptr<CImageObject>				m_pSpriteStartImg;
	//選択肢表示用板ポリ.
	std::shared_ptr<CChoiceImage>				m_pSpriteChoiceImg;
	//YES&NO表示用板ポリ.
	std::shared_ptr<CSelectIcon>				m_pSpriteYesSelectImg;
	std::shared_ptr<CSelectIcon>				m_pSpriteNoSelectImg;

	//シーンタイプ変数.
	CSceneType					m_SceneType;


	//試し.
	D3DXVECTOR2 m_UV = { 1.f, 1.f };

	//キー入力.
	std::shared_ptr<CMultiInputKeyManager>		m_InputKey;




	//描画フラグ.
	bool DrawFlag;

private:


};