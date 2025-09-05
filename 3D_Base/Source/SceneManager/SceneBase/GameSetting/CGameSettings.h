#pragma once
//-----継承するクラス-----
#include "SceneManager//SceneBase//CSceneBase.h" // シーン基底クラス

//-----ライブラリ-----
#include <iostream>
#include <windows.h>

#include "Global.h"

// -----外部クラス-----
#include "Assets//DirectX//DirectX9//CDirectX9.h" // DirectX9クラス
#include "Assets//DirectX//DirectX11//CDirectX11.h" // DirectX11クラス

#include "GameObject//UI//ImageObject//CImageObject.h"	// 画像オブジェクトクラス
#include "GameObject//UI//CUIObject//CUIObject.h"		// UIオブジェクトクラス

#include "GameObject//UI//ChoiceImage//CChoiceImage.h"  // 選択肢クラス
#include "GameObject//UI//SelectIcon//CSelectIcon.h"	// 選択アイコンクラス

#include "Assets//Sprite//Sprite2D//CSprite2D.h" // 2Dスプライトクラス.

//-----メッシュ-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // スタティックメッシュクラス

//キー入力用.
#include "InputDevice//Matuoka//CMultiInputKeyManager.h" // キー入力用


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