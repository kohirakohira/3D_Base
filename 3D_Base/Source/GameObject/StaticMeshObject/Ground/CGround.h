#pragma once
//-----継承するクラス-----
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" // スタティックメッシュオブジェクトクラス 

//-----外部クラス-----
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerManager/CPlayerManager.h"

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

	// リスポーン可能エリアに
	// プレイヤーの座標を指定する
	void RespawnArea();

	// マップの中央を跨がないように計算する
	int GetAreaIndex(float x, float z);

	//プレイヤーを設定.
	void SetPlayer(std::shared_ptr<CPlayerManager> pPlayer) { m_pPlayerManager = pPlayer; }

protected:
	//プレイヤーマネージャー.
	std::shared_ptr<CPlayerManager>		m_pPlayerManager;

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