#pragma once
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" /* 継承クラス || スタティックメッシュオブジェクトクラス */
#include <iostream>

#include "GameObject//StaticMeshObject//ItemBoxManager//ItemBoxType//ItemType.h" // アイテムタイプ

///------------------------------------------------------
/// アイテムボックス
///------------------------------------------------------

class CItemBox
	:public CStaticMeshObject // スタティックメッシュオブジェクトクラスを継承.
{
public:
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

public:
	CItemBox();
	~CItemBox()override;

	// 動作関数.
	void Update() override;
	// 描画関数.
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	// どれだけ早くなるか、速度を取得.
	void GravityMath();

	// アイテムの情報を設定する.
	void SetItemInfo(CItemType item);

	//アイテム情報を渡す用.
	ItemEffect GetItem() const { return m_Item; }

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

	// 当たった時の処理
	void HitPlayer();

	// バウンディングボックスを作成
	void CreateBounding(std::shared_ptr<CStaticMesh> pItemBox);

protected:
	//初期速度.
	float InitialSpeed;
	//重力加速度(9.8m/sの2乗だけど、下向きに落ちるので-を付ける).
	float GravitySpeed;
	//１フレーム(0.016 : 1/60FPS).
	float Framerate;
	
	//	アクティブフラグ(アイテムが取られたかどうか).
	bool m_Active;

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