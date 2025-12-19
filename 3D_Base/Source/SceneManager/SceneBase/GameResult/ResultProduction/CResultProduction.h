#pragma once
#include <iostream>
#include <vector>

//-----外部クラス-----
#include "../../../../Camera/CCamera.h" //カメラクラス.
#include "../../../../Assets/BackGround/BackGround.h"	//背景クラス.
#include "../../../../GameObject/SpriteObject/CSpriteObject.h"	//スプライトオブジェクトクラス.
//キャラクターマネージャー.
#include "../../../../GameObject/StaticMeshObject/Character/CharacterObject/Player/CharacterManager/CCharacterManager.h"

//-----メッシュクラス-----
#include "../../../../Assets/Mesh/StaticMesh/CStaticMesh.h" //スタティックメッシュクラス.
#include "../../../../Assets/Sprite/Sprite3D/CSprite3D.h" //地面スプライト.


//======================================================
//	リザルトの演出.
//======================================================
class CResultProduction
{
public:
	CResultProduction();
	~CResultProduction();

	//生成関数.
	void Create();
	//初期化.
	void Init();
	//読み込み関数.
	HRESULT LoadData();

public:
	//勝ち抜け.
	void WinUpdate();
	void WinDraw();
	//引き分け.
	void DrawUpdate();
	void DrawDraw();

private:
	//キャラクターの基準位置.
	float m_CharaPosX;

	//カメラクラス変数.
	std::shared_ptr<CCamera>		m_Camera;

	//スタティックメッシュ.
	std::vector<std::shared_ptr<CStaticMesh>>	m_BodyMesh;
	std::vector<std::shared_ptr<CStaticMesh>>	m_CannonMesh;
	//スプライト3D.
	std::shared_ptr<CSprite3D>		m_SpriteGround;
	std::shared_ptr<CSprite3D>		m_SpriteBackGround;

	//スプライトオブジェクト.
	//背景クラス変数.
	std::unique_ptr<BackGround>			m_BackGround;
	std::unique_ptr<CSpriteObject>		m_SpriteObjGround;
	std::shared_ptr<CCharacterManager>	m_CharacterManager;
};