#pragma once
#include <iostream>
#include <algorithm>

//アセットクラス(当たり判定).
#include "Collision//Collider//CCollider.h"
#include "GameObject//StaticMeshObject//CStaticMeshObject.h"

//爆発のクラス.
//※スタティックメッシュオブジェクトを継承.
class CBlastCollision
	:public CStaticMeshObject
{
public:
	CBlastCollision();
	~CBlastCollision();

	//動作処理.
	void Update() override;
	//描画処理.
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	//当たった時の関数.
	void HitBlast();

	//爆発フラグ設定.
	void SetBlastFlag(bool flg) { m_Bom = flg; }
	//爆発フラグの取得.
	bool GetBlastFlag() { return m_Bom; }
	//半径の取得.
	float GetBlastRadius() { return m_Radius; }

public:
	//半径.
	float	m_Radius;
	//爆発フラグ.
	bool	m_Bom;
};