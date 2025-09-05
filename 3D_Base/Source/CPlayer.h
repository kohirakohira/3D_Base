#pragma once
#include <iostream>
#include "CCharacter.h"

#include "CBody.h"
#include "CCannon.h"
#include <memory>
#include <vector>

class CPlayer
	: public CCharacter
{
public:
	CPlayer();
	virtual ~CPlayer() override;

	virtual void Initialize(int id);

	void AttachMeshse(std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon) ;
	void SetTankPosition(const D3DXVECTOR3& pos);
	void SetTankRotation(const D3DXVECTOR3& pos);

	//Body優先でワールド座標と回転を返す
	virtual D3DXVECTOR3 GetPosition() const;
	virtual D3DXVECTOR3 GetRotation() const;

	virtual void Update() override;
	virtual void Draw(
		D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

   	//外部のクラスから情報取得.
	void SetCBody(std::shared_ptr<CBody> pBody) { m_pBody = pBody; }
	void SetCCannon(std::shared_ptr<CCannon> pCannon) { m_pCannon = pCannon; }

	// 外部のクラスに情報を渡す
	std::shared_ptr<CCannon> GetCannon() const { return m_pCannon; }
	std::shared_ptr<CCannon> GetCannon() { return m_pCannon; }
	std::shared_ptr<CBody>	 GetBody()   const { return m_pBody; }

	float GetCannonYaw() const;
	D3DXVECTOR3 GetCannonPosition() const;


protected:

	std::shared_ptr<CBody>		m_pBody;
	std::shared_ptr<CCannon>	m_pCannon;
	int			m_Hp;
	int			m_PlayerID;

protected:
	std::shared_ptr<CBody> Body() const { return m_pBody; }
	std::shared_ptr<CCannon> Cannon() const { return m_pCannon; }
};
