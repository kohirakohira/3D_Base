#pragma once
#include "GameObject//StaticMeshObject//Character//CCharacter.h" /* 継承クラス || キャラクタークラス */
#include <vector>
#include <memory>

//-----メッシュ-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // スタティックメッシュクラス
#include "GameObject//StaticMeshObject//Shot//CShot.h" // ショットクラス

class CShotManager 
	: public CCharacter // キャラクタークラスを継承.
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
