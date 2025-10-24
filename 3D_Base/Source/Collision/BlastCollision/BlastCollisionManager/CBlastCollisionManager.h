#pragma once
#include <iostream>
#include <vector>

//爆風クラスマネージャー.
#include "Collision//BlastCollision//CBlastCollision.h"

//=========================================================
//	爆風クラスのマネージャー.
//=========================================================
class CBlastCollisionManager
{
public:
	CBlastCollisionManager();
	~CBlastCollisionManager();

	//動作処理.
	void Update();
	//描画処理.
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera);
	//インスタンス生成.
	void Create(const D3DXVECTOR3& pos, bool blast, std::shared_ptr<CStaticMesh> mesh);

	//当たった時の関数.
	void HitBlast();

	//爆風メッシュのアタッチ.
	void AttachMesh(std::shared_ptr<CStaticMesh> mesh);
	//モデルに合わせたバウンディングスフィア作成のラッパー関数
	void CreateBSphereForMesh(std::shared_ptr<CStaticMesh> mesh);
	//スフィアのコライダーの生成.
	void CreateSpehreCollider(float rad);

	//位置の設定.
	void SetPosition(D3DXVECTOR3 pos);
	void SetPosition(float x, float y, float z);
	//回転の設定.
	void SetRotation(D3DXVECTOR3 rot);
	void SetRotation(float x, float y, float z);
	//大きさの設定.
	void SetScale(float xyz );
	//爆発フラグ設定.
	void SetBlastFlag(bool flg);
	//爆発フラグの取得.
	bool GetBlastFlag();
	//半径の取得.
	float GetBlastRadius();
	//コライダーの取得.
	std::shared_ptr<CCollider> GetCollider();

public:
	//爆風クラスの変数.
	std::vector<std::unique_ptr<CBlastCollision>>	m_pBlastCollision;

};