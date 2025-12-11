#pragma once
//-----ライブラリ.-----
#include <iostream>
#include <algorithm>

//-----継承クラス.-----
#include "../../CStaticMeshObject.h"

//=============================================================
//		爆発クラス.
//=============================================================
class CBlast
	:public CStaticMeshObject
{
public:
	CBlast();
	~CBlast() override final;

	//更新処理.
	void Update() override;

	//描画処理.
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	//半径の最大サイズを設定.
	void SetMaxRadius(float rad);

	//秒数の設定.
	void SetSpeed(float s);

	//爆発フラグの設定.
	void SetBomStart(bool f);

	//プレイヤーID設定・取得.
	void SetPlayerID(float id);
	float GetPlayerID() const { return m_PlayerID; }

	//爆発の開始フラグを取得.
	bool IsBomStart() const;
	//爆発の終了フラグを取得.
	bool IsBomFinish() const;

	//半径の取得.
	float GetBlastRadius() { return m_Radius; }

public:
	//半径の最大サイズ.
	float	m_MaxSize;
	//爆発フラグ.
	bool	m_BomStart;
	bool	m_BomFinish;
	//速度.
	float	m_Speed;
	//プレイヤーID.
	float	m_PlayerID;

private:
	//半径の最低サイズ.
	float	m_MinimumSize;
	//半径.
	float	m_Radius;

};