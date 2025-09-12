#pragma once
//-----ライブラリ-----
#include <d3dx9.h>
#include <vector>
#include <memory>

//-----前方宣言-----
class CGameObject; // ゲームオブジェクトクラス

//===================================
//	基底コライダークラス
//===================================
class CCollider
	: public std::enable_shared_from_this<CCollider> // shared_from_this()を使うために継承
{
public:
	//-----列挙型-----
	enum class ColliderType
	{
		Sphere,		// 球
		Box,		// 矩形
	};

public:
	CCollider(std::shared_ptr<CGameObject> owner, ColliderType type);
	virtual ~CCollider() {}

	// タイプの取得
	ColliderType GetType() const { return m_Type; }

	// オーナーの取得
	std::shared_ptr<CGameObject> GetOwner() const { return m_pOwner; }

	// 衝突判定インターフェース
	virtual bool CheckCollision(std::shared_ptr<CCollider> other) const = 0;

protected:
	std::shared_ptr<CGameObject> m_pOwner; // このコライダーを持つオブジェクト
	ColliderType m_Type; // コライダーのタイプ
};

