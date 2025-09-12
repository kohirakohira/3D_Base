#pragma once
//-----ライブラリ-----
#include <vector>
#include <memory>

//-----外部クラス-----
#include "Collision/Collider/CCollider.h" // コライダークラス
#include "Collision//Collider//SphereCollider//CSphereCollider.h" // スフィアコライダークラス
#include "Collision//Collider//BoxCollider//CBoxCollider.h" // ボックスコライダークラス

//===================================
//	ゲームオブジェクトクラス.
//===================================
class CGameObject
	: public std::enable_shared_from_this<CGameObject>
{
public:
	CGameObject();
	//継承を行う場合は、デストラクタにvirtualを付けて仮想関数にすること.
	virtual ~CGameObject();

	//virtual 型 関数名 = 0; 純粋仮想関数.
	//子クラスに処理をお任せするので、ここでは名前だけ宣言して定義は書かない.
	virtual void Update() = 0;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera ) = 0;

	// --- Collider 管理 ---
	// スフィアコライダー追加
	std::shared_ptr<CSphereCollider> AddSphereCollider(float radius)
	{
		// shared_ptrに変換して管理
		auto collider = std::make_shared<CSphereCollider>(shared_from_this(), radius);

		// CCollider の vector に shared_ptr を格納する場合は型を合わせる
		m_Colliders.push_back(collider);

		return collider;
	}

	const std::vector<std::shared_ptr<CCollider>>& GetColliders() const { return m_Colliders; }


	//座標設定関数.
	void SetPosition( float x, float y, float z ){
		m_vPosition.x = x;
		m_vPosition.y = y;
		m_vPosition.z = z;
	};
	void SetPosition(const D3DXVECTOR3& pos) {
		m_vPosition = pos;
	}
	//座標取得関数.
	const D3DXVECTOR3& GetPosition() const {
		return m_vPosition;
	}

	//回転設定関数.
	void SetRotation( float x, float y, float z ){
		m_vRotation.x = x;
		m_vRotation.y = y;
		m_vRotation.z = z;
	};
	void SetRotation(const D3DXVECTOR3& rot) {
		m_vRotation = rot;
	}
	//回転取得関数.
	const D3DXVECTOR3& GetRotation() const {
		return m_vRotation;
	}

	//拡縮設定関数.
	void SetScale( float x, float y, float z ){
		m_vScale.x = x;
		m_vScale.y = y;
		m_vScale.z = z;
	}
	void SetScale(float xyz) {
		m_vScale = D3DXVECTOR3(xyz, xyz, xyz);
	}
	//拡縮取得関数.
	const D3DXVECTOR3& GetScale() const {
		return m_vScale;
	}

protected://protectedは子クラスのみアクセス可能.
	D3DXVECTOR3	m_vPosition;
	D3DXVECTOR3	m_vRotation;
	D3DXVECTOR3	m_vScale;

	std::vector<std::shared_ptr<CCollider>> m_Colliders; // 複数コライダー保持
};