#pragma once
#include <iostream>

#include "GameObject//StaticMeshObject//CStaticMeshObject.h" /* 継承クラス || スタティックメッシュオブジェクトクラス */
#include "GameObject//StaticMeshObject//ItemBoxManager//ItemBoxType//ItemType.h" // アイテムタイプ
#include "Global.h"
//エフェクト.
#include "Assets/Effect/CEffect.h"

///------------------------------------------------------
/// アイテムボックス
///------------------------------------------------------

class CItemBox
	:public CStaticMeshObject // スタティックメッシュオブジェクトクラスを継承.
{
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
	//重力の有無を設定する関数.
	void SetGravity(bool flg) { IsGravity = flg; }
	//重力の有無取得.
	bool GetGravity() { return IsGravity; }
	//アイテム情報を渡す用.
	ItemInfomation GetItem() const { return m_Item; }

	//アイテムの種類を取得する関数.
	CItemType GetItemInfo() { return m_ItemType; }

	//アクティブフラグの取得関数.
	bool IsActive() { return m_Active; }

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

	//エフェクトの始まり終わり.
	void StartEffect();

protected:
	//重力があるかないか.
	bool IsGravity;

	//アイテムの煙.
	bool ItemFlag;

	//初期速度.
	float InitialSpeed;
	//重力加速度(9.8m/sの2乗だけど、下向きに落ちるので-を付ける).
	float GravitySpeed;
	//１フレーム(0.016 : 1/60FPS).
	float Framerate;
	
	//	アクティブフラグ(アイテムが取られたかどうか).
	bool m_Active;

	//アイテムの情報.
	ItemInfomation m_Item;

	//アイテムの種類.
	CItemType m_ItemType;

	//エフェクトのインスタンスごとに必要なハンドル
	//※３つ表示して制御するなら３つ必要になる
	::EsHandle hEffect;


};