#pragma once
#include <iostream>

//-----外部クラス-----
#include "../../../../GameObject/SpriteObject/CSpriteObject.h"	//スプライトオブジェクトクラス.
#include "../../../../Camera/CCamera.h" //カメラクラス.
#include "../../../../GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankBody/CBody.h"
#include "../../../../GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankCannon/CCannon.h"
#include "../../../../Assets/BackGround/BackGround.h"

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

	//地面の動き.
	void MoveGround();

public:
	//円周率.
	const float PI = 3.141592;

	//UVを動かす用.
	D3DXVECTOR2 m_UV;

	//スプライトクラス.
	//地面画像.
	std::unique_ptr<CSprite3D>			m_SpriteGround;
	//板ポリ画像.
	std::shared_ptr<CSprite3D>			m_BackGroundImg;

	//カメラクラス.
	std::shared_ptr<CCamera>			m_Camera;

	//スタティックメッシュクラス.
	std::shared_ptr<CStaticMesh>		m_BodyMesh;				//ボディメッシュ.
	std::shared_ptr<CStaticMesh>		m_CannonMesh;			//キャノンメッシュ.
	
	//地面クラス.
	std::unique_ptr<CSpriteObject>		m_SpriteObjGround;
	//空の板ポリ.
	std::unique_ptr<BackGround>			m_BackGround;

	//プレイヤー(車体)クラス.
	std::unique_ptr<CBody>				m_Body;
	//プレイヤー(砲塔)クラス.
	std::unique_ptr<CCannon>			m_Cannon;

};