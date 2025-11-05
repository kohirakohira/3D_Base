#pragma once
//-----ライブラリ-----
#include <vector>
#include <memory>
#include <array>

//-----メッシュ-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // スタティックメッシュクラス
#include "GameObject//StaticMeshObject//Shot//CShot.h" // ショットクラス

class CShotManager 
{
public:
	CShotManager();
	~CShotManager();

	void Initialize();
	void AttachMeshToPlayerShot(BulletKinds kind, std::shared_ptr<CStaticMesh> mesh);
	//void SetReload(int No, const D3DXVECTOR3& pos, float rotY);

	// 動作処理
	void Update() ;
	// 描画処理
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) ;

	// インスタンス生成
	void Create(const D3DXVECTOR3& pos, bool shotFlg, int No);

	// バウンディングオブジェクトの作成
	void CreateBSphereForMesh(std::shared_ptr<CStaticMesh>& mesh, int index);

	// コライダーの作成
	void CreateCollider(int index);

	// 位置の取得
	const D3DXVECTOR3& GetPosition();

	// 外部のクラスから情報取得
	//void SetCShot(std::vector<std::unique_ptr<CShot>> pShot) { m_pShots = pShot; }

	// 外部のクラスに情報を渡す
	//std::vector<std::unique_ptr<CShot>>	GetShot() const { return m_pShots; }

private:
	// メッシュの情報を持つ変数
	std::array<std::shared_ptr<CStaticMesh>, PLAYER_MAX>	m_Mesh;

	// 弾クラス
	std::vector<std::unique_ptr<CShot>> m_pShots;
};
