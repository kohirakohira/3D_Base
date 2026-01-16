#pragma once
//-----継承するクラス-----
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" // スタティックメッシュオブジェクトクラス 

//-----外部クラス-----
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/CharacterManager/CCharacterManager.h"

//==================================
//	グラウンドクラス
//==================================
class CGround
	: public CStaticMeshObject // スタティックメッシュオブジェクトクラスを継承
{
public:
	CGround();
	virtual ~CGround();

	virtual void Update() override;

	//初期化.
	void Init();

protected:
	// キャラクターマネージャークラス
	std::shared_ptr<CCharacterManager>		m_pCharacterManager;

public:
//=============================================================
// 	   演出用.
//=============================================================
	//地面の動き.
	void Move(float speed);


public:
	//地面が動く速度.
	float m_Speed;
};