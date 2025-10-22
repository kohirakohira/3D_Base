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

public:
	//半径.
	float m_Radius;
};