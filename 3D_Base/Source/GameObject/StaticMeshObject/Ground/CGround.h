#pragma once
//-----継承するクラス-----
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" // スタティックメッシュオブジェクトクラス 

//-----外部クラス-----
#include "GameObject/StaticMeshObject/Character/Player/PlayerManager/CPlayerManager.h"

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
};