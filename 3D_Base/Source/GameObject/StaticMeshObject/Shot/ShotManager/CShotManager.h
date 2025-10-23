#pragma once
//-----継承するクラス-----
#include "GameObject//StaticMeshObject//Character//CCharacter.h" //キャラクタークラス 

//-----ライブラリ-----
#include <vector>
#include <memory>

//-----メッシュ-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // スタティックメッシュクラス
#include "GameObject//StaticMeshObject//Shot//CShot.h" // ショットクラス

class CShotManager 
	//: public CCharacter // キャラクタークラスを継承.
{
public:
	CShotManager();
	~CShotManager();

	void Initialize(int playerCount);
	void AttachMeshToPlayerShot(int playerIndex, std::shared_ptr<CStaticMesh> mesh);
	void SetReload(int playerIndex, const D3DXVECTOR3& pos, float rotY);

	void Update() ;
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) ;

	// バウンディングオブジェクトの作成
	void CreateBounding(std::shared_ptr<CStaticMesh>& pShot);

	// コライダーの作成
	void CreateCollider();

	//位置の取得.
	const D3DXVECTOR3& GetPosition();

	//外部のクラスから情報取得.
	void SetCShot(std::vector<std::shared_ptr<CShot>> pShot) { m_pShots = pShot; }

	// 外部のクラスに情報を渡す
	std::vector<std::shared_ptr<CShot>>	GetShot() const { return m_pShots; }

private:
	std::vector<std::shared_ptr<CShot>> m_pShots;
};
