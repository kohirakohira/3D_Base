#pragma once
//-----継承するクラス-----
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" // スタティックメッシュオブジェクトクラス 

//-----外部クラス-----
#include "GameObject//StaticMeshObject//Character//CCharacter.h" // キャラクタークラス

//==================================
//	ステージクラス
//==================================
class CGround
	: public CStaticMeshObject // スタティックメッシュオブジェクトクラスを継承
{
public:
	CGround();
	virtual ~CGround();

	virtual void Update() override;

	//プレイヤーを設定.
	void SetPlayer( CCharacter& pPlayer ) { m_pPlayer = &pPlayer; }

protected:
	CCharacter* m_pPlayer;

};