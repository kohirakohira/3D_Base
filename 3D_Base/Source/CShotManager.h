#pragma once
#include "CCharacter.h"

#include "CShot.h"
#include "CStaticMesh.h"
#include <vector>
#include <memory>

class CShotManager : public CCharacter
{
public:
	CShotManager();
	~CShotManager();

	void Initialize(int playerCount);
	void AttachMeshToPlayerShot(int playerIndex, std::shared_ptr<CStaticMesh> mesh);
	void SetReload(int playerIndex, const D3DXVECTOR3& pos, float rotY);

	void Update() override;
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

private:
	std::vector<std::vector<std::unique_ptr<CShot>>> m_pShots;
};
