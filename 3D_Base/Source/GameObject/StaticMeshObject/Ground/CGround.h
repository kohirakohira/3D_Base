#pragma once
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" /* 継承クラス || スタティックメッシュオブジェクトクラス */

//-----外部クラス-----
#include "GameObject//StaticMeshObject//Character//CCharacter.h" // キャラクタークラス

/*********************************************************
*	地面クラス
**/
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