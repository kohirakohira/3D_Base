#pragma once
#include <iostream>

//-----外部クラス-----
#include "../../../../Camera/CCamera.h" //カメラクラス.
#include "../../../../GameObject/StaticMeshObject/Ground/CGround.h" //地面クラス.

//-----メッシュクラス-----
#include "../../../../Assets/Mesh/StaticMesh/CStaticMesh.h" //スタティックメッシュクラス.


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
	//スタティックメッシュクラス.
	std::shared_ptr<CStaticMesh>	m_GroundMesh;		//地面メッシュ.

	//カメラクラス変数.
	std::shared_ptr<CCamera>		m_Camera;
	//地面クラス変数.
	std::unique_ptr<CGround>		m_Ground;

};