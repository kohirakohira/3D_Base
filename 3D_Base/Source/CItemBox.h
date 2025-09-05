#pragma once

#include <iostream>


#include "CStaticMeshObject.h"
#include "ItemType.h"

///------------------------------------------------------
/// アイテムボックス(スタティックメッシュオブジェを継承).
///------------------------------------------------------

class CItemBox
	:public CStaticMeshObject
{
public:
	CItemBox();
	~CItemBox()override;

	//動作関数.
	void Update() override;
	//描画関数.
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	//どれだけ早くなるか、速度を取得.
	void GravityMath();

	//アイテムの情報を設定する.
	void SetItemInfo(CItemType item);

	//アイテムの種類を取得する関数.
	CItemType GetItemInfo() { return m_ItemType; }





public:
	//各アイテムごとの効果.
	//シールド効果.
	void ShieldEffect();
	//速度UP効果.
	void SpeedUpEffect();
	//攻撃UP効果.
	void PowerUpEffect();
	//爆風範囲増加効果.
	void BlastUpEffect();
	//反射効果.
	void ReflectionEffect();
	//リロード効果.
	void ReloadEffect();

protected:
	//初期速度.
	float InitialSpeed;
	//重力加速度(9.8m/sの2乗だけど、下向きに落ちるので-を付ける).
	float GravitySpeed;
	//１フレーム(0.016 : 1/60FPS).
	float Framerate;
	
	//アイテムの種類.
	CItemType m_ItemType;

	//アイテム効果の構造体.
	struct ItemEffect
	{
		bool	m_ShieldFlag = false;		//シールドを張っているかどうか.
		float	m_Speed;					//プレイヤーの速度変更用.
		float	m_Power;					//プレイヤーの攻撃変更用.
		float	m_Blast;					//プレイヤーの爆風増加変更用.
		bool	m_Reflection = false;		//プレイヤーの反射変更用.
		float	m_Reload;					//プレイヤーのリロード間隔変更用.
	}m_Item;


};