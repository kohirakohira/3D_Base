#pragma once
#include <iostream>

//-----外部クラス-----
#include "../../../../GameObject/SpriteObject/CSpriteObject.h"	//スプライトオブジェクトクラス.
#include "../../../../Camera/CCamera.h" //カメラクラス.

//-----メッシュクラス-----
#include "../../../../Assets/Mesh/StaticMesh/CStaticMesh.h" //スタティックメッシュクラス.
#include "../../../../GameObject/StaticMeshObject/CStaticMeshObject.h" // スタティックメッシュオブジェクトクラス.
#include "../../../../Assets/Sprite/Sprite3D/CSprite3D.h" //地面スプライト.

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
	//円周率.
	const float PI = 3.141592;

	//カメラクラス.
	std::shared_ptr<CCamera>			m_Camera;

	//スタティックメッシュクラス.
	std::shared_ptr<CStaticMesh>		m_BackGroundMesh;	//背景メッシュ.
	//スプライトクラス.
	std::unique_ptr<CSprite3D>			m_SpriteGround;

	//背景.
	std::unique_ptr<CStaticMeshObject>	m_pBackImgObject;
	//地面クラス.
	std::unique_ptr<CSpriteObject>		m_SpriteObjGround;

public:
	float rad;

};