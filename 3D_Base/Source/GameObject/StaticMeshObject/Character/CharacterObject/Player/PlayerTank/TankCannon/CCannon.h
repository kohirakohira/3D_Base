#pragma once
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" // 継承 

//-----外部クラス-----
#include "Camera//CCamera.h" //カメラクラス
#include "InputDevice//Input//Controller//CController.h" // コントローラークラス
#include "GameObject//StaticMeshObject//Shot//ShotManager//CShotManager.h"

#include <memory>

#if 0
struct CannonRayResult
{
	bool        bHit = false;               // 何かにヒットしたか
	float       distance = 0.0f;            // ヒット地点までの距離
	D3DXVECTOR3 hitPoint = { 0, 0, 0 };       // ヒット地点のワールド座標
	CStaticMeshObject* pHitObject = nullptr; // ヒットしたオブジェクト
};

#endif

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
	// 描画関数
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;
	// 初期化関数
	void Init();

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

	//レイの取得.
	RAY GetRay() const{ return *m_pRay; }


#if 0
	// レイ関連
	// レイの初期化（ゲーム開始時に1回呼ぶ）
	HRESULT InitCannonRay(float length = 50.0f);

	// レイの更新（毎フレーム呼ぶ）
	void UpdateCannonRay();

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
	bool RaycastTo(CStaticMeshObject* pTarget, CannonRayResult& outResult) const;

	// 複数オブジェクトへのレイキャスト（最も近いものを返す）
	bool RaycastToNearest(
		const std::vector<CStaticMeshObject*>& targets,
		CannonRayResult& outResult) const;

	// 指定位置が射線上にあるか（簡易判定）
	bool IsPositionInSight(const D3DXVECTOR3& targetPos, float toleranceAngle = 0.1f) const;

	//========================================
	// デバッグ描画
	//========================================
	// レイの描画を有効/無効
	void SetRayVisible(bool visible) { m_bDrawRay = visible; }
	bool IsRayVisible() const { return m_bDrawRay; }

	// レイのデバッグ描画
	void DrawRay(D3DXMATRIX& View, D3DXMATRIX& Proj);
#endif
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

	//レイ構造体のポインタ
	RAY* m_pRay;

#if 0
	RAY         m_CannonRay;                // レイ構造体（判定用）
	std::unique_ptr<CRay> m_pRayDrawer;     // レイ描画クラス（デバッグ用）
	float       m_MuzzleOffset = 2.0f;      // 砲口の前方オフセット
	bool        m_bDrawRay = false;         // レイを描画するか
#endif
};