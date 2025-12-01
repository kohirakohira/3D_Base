#pragma once
#include <iostream>

//-----外部クラス-----
#include "Camera//CCamera.h" //カメラクラス.
#include "GameObject//StaticMeshObject//Character//Player//PlayerManager//CPlayerManager.h"	//プレイヤーマネージャークラス.
#include "../../../../GameObject/SpriteObject/CSpriteObject.h"	//スプライトオブジェクトクラス.

//-----メッシュクラス-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" //スタティックメッシュクラス.
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
	//読み込み関数.
	HRESULT LoadData();

public:
	//スタティックメッシュクラス.
	std::shared_ptr<CStaticMesh>	m_GroundMesh;		//地面メッシュ.
	std::shared_ptr<CStaticMesh>	m_PlayerMesh;		//プレイヤーメッシュ.
	//スプライトクラス.
	std::unique_ptr<CSprite3D>		m_SpriteGround;

	//カメラクラス.
	std::shared_ptr<CCamera>		m_Camera;
	//地面クラス.
	std::unique_ptr<CSpriteObject>	m_SpriteObjGround;
	//プレイヤークラス.
	std::unique_ptr<CPlayerManager> m_Player;

};