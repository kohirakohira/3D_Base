#pragma once
#include "GameObject//StaticMeshObject//Character//CCharacter.h" /* �p���N���X || �L�����N�^�[�N���X */
#include <vector>
#include <memory>

//-----���b�V��-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // �X�^�e�B�b�N���b�V���N���X
#include "GameObject//StaticMeshObject//Shot//CShot.h" // �V���b�g�N���X

class CShotManager 
	: public CCharacter // �L�����N�^�[�N���X���p��.
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
