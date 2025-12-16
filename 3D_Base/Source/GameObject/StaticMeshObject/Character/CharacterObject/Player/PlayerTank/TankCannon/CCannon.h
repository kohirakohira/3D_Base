#pragma once
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" // 継承 

//-----外部クラス-----
#include "Camera//CCamera.h" //カメラクラス
#include "InputDevice//Input//Controller//CController.h" // コントローラークラス
#include "GameObject//StaticMeshObject//Shot//ShotManager//CShotManager.h"

#include <memory>

// レイヒット時の情報
struct CannonHitRay
{
	bool bHit = true;								// 何にヒットしたか
	float Distance = 0.f;							// 距離
	D3DXVECTOR3 HitPoint{0.f,0.f,0.f};				// ヒット位置
	CStaticMeshObject* pHitObject = nullptr;		// ヒットしたオブジェクト
};

/**************************************************
*	砲塔クラス.
**/
class CCannon
	: public CStaticMeshObject	//スタティックメッシュオブジェクトを継承.
{
public:
	CCannon(int inputID);
	~CCannon();

	// 更新関数
	void Update() override;

	// レイの更新
	void UpdateCannonRay();

	// 描画関数
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;
	// 初期化関数
	void Init();

	//レイ初期化.呼び出すinitがHRESULTなのでHRESULT型
	HRESULT InitCannonRay(float len = 60.f);

	// 砲塔座標設定
	void SetCannonPosition(const D3DXVECTOR3& Pos);

	// 砲塔の情報を渡す
	D3DXVECTOR3 GetCannonPosition() const { return m_vPosition; }

	//プレイヤーが壁に当たると戻す.
	void PushBack(const D3DXVECTOR3& push);

	// バウンディングボックスを作成
	void CreateBounding(std::shared_ptr<CStaticMesh> pCannon);

	// 弾クラスの設定
	void SetShotManager(const std::shared_ptr<CShotManager>& shot) { m_pShot = shot; }

	//弾を作成・リロード・発射関数.
	void Reload(D3DXVECTOR3 pos, float y, bool flag, int index);

	// レイの取得（読み取り専用）
	const RAY& GetCannonRay() const { return m_CannonRay; }

	// レイの長さを設定
	void SetRayLength(float length) { m_CannonRay.Length = length; }
	float GetRayLength() const { return m_CannonRay.Length; }

	// 砲塔の前方ベクトルを取得
	D3DXVECTOR3 GetForward() const;

	// 砲口のワールド座標を取得
	D3DXVECTOR3 GetMuzzlePosition() const;

	// 砲口オフセットの設定
	void SetMuzzleOffset(float offset) { m_MuzzleOffset = offset; }

	// レイキャスト判定
	// 単一オブジェクトへのレイキャスト
	bool RaycastTo(CStaticMeshObject* pTarget, CannonHitRay& outResult) const;
	
	//複数オブジェクトに対するレイキャスト
	bool RaycastToNearest(
		const std::vector<CStaticMeshObject*>& targets,
		CannonHitRay& outResult) const;

	// 指定位置が射線上にあるか
	bool IsPositionInSight(const D3DXVECTOR3& targetPos, float toleranceAngle = 0.1f) const;

	// レイの描画を有効/無効
	void SetRayVisible(bool visible) { m_DrawRay = visible; }
	bool IsRayVisible() const { return m_DrawRay; }

	// 射線上に障害物があるか
	bool HasObstacleInFireLine(const D3DXVECTOR3& targetPos,
		const std::vector<CStaticMeshObject*>& obstacles) const;

	// 射線が通っているか（障害物なし && 射線上）
	bool CanFireAt(const D3DXVECTOR3& targetPos,
		const std::vector<CStaticMeshObject*>& obstacles,
		float toleranceAngleDeg = 10.0f) const;

	// レイのデバッグ描画
	void DrawRay(D3DXMATRIX& View, D3DXMATRIX& Proj);

	bool RaycastToPosition(const D3DXVECTOR3& targetPos, float targetRadius, float& outDistance) const;

private:
	float		m_TurnSpeed;			// 回転速度

	int			m_ShotCoolTime;			// ショットのクールタイム
	const int	m_ShotInterval;			// ショットのインターバル

	// 弾クラス
	std::shared_ptr<CShotManager>		m_pShot;

	// プレイヤーの番号を管理するための変数
	int 			m_PlayerID;				// 入力ID

	// コントローラークラス
	CController* m_pController;

	//レイ構造体
	RAY m_CannonRay;
	//レイの描画クラス	
	std::unique_ptr<CRay> m_pRayDrawer;

	//砲塔のオフセット
	float m_MuzzleOffset;

	//レイを描画するか
	bool m_DrawRay;

};