#pragma once
//-----継承するクラス-----
#include "Collision//Collider//CCollider.h" // コライダークラス

//===================================
// ボックスコライダークラス (AABB<とりあえず>)
//===================================
class CBoxCollider
	: public CCollider // コライダークラスを継承
{
public:
	CBoxCollider(
		std::shared_ptr<CGameObject> owner,
		const D3DXVECTOR3& size);

	virtual ~CBoxCollider() override;

	// サイズの取得
	const D3DXVECTOR3& GetSize() const { return m_Size; }

	// 衝突判定インターフェースの実装
	virtual bool CheckCollision(std::shared_ptr<CCollider> other) const override;

private:
	D3DXVECTOR3 m_Size; // ボックスのサイズ (幅、高さ、奥行き)
};