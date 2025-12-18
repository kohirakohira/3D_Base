#pragma once
#include <iostream>

//-----外部クラス-----
#include "../../../../Camera/CCamera.h" //カメラクラス.




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
	//勝ち抜け.
	void WinUpdate();

	//引き分け.
	void DrawUpdate();

private:
	//スタティックメッシュクラス.
	std::shared_ptr<CStaticMesh>	m_GroundMesh;		//地面メッシュ.

	//スプライト3D.
	std::shared_ptr<CSprite3D>		m_SpriteGround;

	//カメラクラス変数.
	std::shared_ptr<CCamera>		m_Camera;
	//地面クラス変数.
	std::unique_ptr<CGround>		m_Ground;

};