#pragma once
#include <iostream>

//-----外部クラス-----.
#include "../../../../Camera/CCamera.h" //カメラクラス.
#include "../../../../GameObject/StaticMeshObject/Character/TitleOnlyPlayer/CTitleOnlyPlayer.h"	//プレイヤークラス.
#include "../../../../GameObject/UI/ImageObject/CImageObject.h"	//画像クラス.

//-----メッシュクラス-----.
#include "../../../../Assets/Mesh/StaticMesh/CStaticMesh.h" //スタティックメッシュクラス.
#include "../../../../GameObject/StaticMeshObject/CStaticMeshObject.h" // スタティックメッシュオブジェクトクラス.

//-----2Dスプライトクラス-----.
#include "../../../../Assets/Sprite/Sprite2D/CSprite2D.h"

//======================================================
//	タイトルの演出.
//======================================================
class CTitleProduction
{
public:
	CTitleProduction();
	~CTitleProduction();

	//動作関数.
	void Update();
	//描画関数.
	void Draw();
	//生成関数.
	void Create();
	//初期化.
	void Init();
	//読み込み関数.
	HRESULT LoadData();

public:
	//2Dスプライト画像.
	std::shared_ptr<CSprite2D>			m_SpriteGroundImage;//地面画像.
	//スタティックメッシュクラス.
	std::shared_ptr<CStaticMesh>		m_BackGroundMesh;	//背景メッシュ.

	//カメラクラス変数.
	std::shared_ptr<CCamera>			m_Camera;
	//背景.
	std::unique_ptr<CStaticMeshObject>	m_pBackImgObject;
	//キャラクタークラス.
	std::unique_ptr<CTitleOnlyPlayer>	m_Player;

	//2Dスプライトクラス.
	std::shared_ptr<CImageObject>		m_Ground;			//地面.

};