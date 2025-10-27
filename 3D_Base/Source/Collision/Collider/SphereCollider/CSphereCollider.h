#pragma once
#include "Collision//Collider//CCollider.h"

class CSphereCollider
	: public CCollider
{
public: // \‘¢‘Ì
	struct SPHERE
	{
		D3DXVECTOR3 CenterPos;
		float Radius;
	} m_Sphere;

public:
	CSphereCollider();
	virtual~CSphereCollider() override;

	// s—ñ•ÏŠ·‚ğXV
	void UpdateTransform(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& scale) override;

	//©g‚ÌŒ^‚ªSphere‚È‚Ì‚Å‘Šè‚ÌŒ^‚ÌCheckCollisionSphere‚ğ’Ê‚é.
	bool CheckCollision(const CCollider& other)const override
	{
		return other.CheckCollisionSphere(*this);
	}

	bool CheckCollisionSphere(const class CSphereCollider& sphere)const override;
	bool CheckCollisionBox(const class CBoxCollider& box)const override;

	//’†SÀ•W‚ğæ“¾‚·‚é
	const D3DXVECTOR3& GetPosition() const override { return m_CenterPos; }
	//”¼Œa(’·‚³)‚ğæ“¾‚·‚é
	float GetRadius() const { return m_Radius; }

	//’†SÀ•W‚ğİ’è‚·‚é
	void SetPosition(const D3DXVECTOR3& Pos) override { m_CenterPos = Pos; }
	
	//”¼Œa(’·‚³)‚ğİ’è‚·‚é
	void SetRadius(float radius);

	//©g‚ÌŒ^‚ª‰½‚ÌŒ^‚©‚ğ•Ô‚·.
	ColliderType GetColType()const override { return ColliderType::Sphere; }

private:
	float	m_Radius;
	float	m_BaseRadius; // ‰Šú‰»‚ÉŒvZ‚³‚ê‚é”¼Œa
};