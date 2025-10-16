#pragma once
#include "Collision//Collider//CCollider.h"

class CSphereCollider
	: public CCollider
{
public:
	CSphereCollider();
	virtual~CSphereCollider() override;

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
	void SetRadius(float Radius) { m_Radius = Radius; }

	//©g‚ÌŒ^‚ª‰½‚ÌŒ^‚©‚ğ•Ô‚·.
	ColliderTipe GetColType()const override { return ColliderTipe::Sphere; }

private:
	float		m_Radius;
};