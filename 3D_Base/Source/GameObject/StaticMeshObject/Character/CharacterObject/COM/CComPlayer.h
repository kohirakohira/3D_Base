//-----継承するクラス-----
#include "GameObject/StaticMeshObject/Character/CharacterObject/CCharacterObject.h"	//基底クラス.

//-----外部のヘッダー-----
//アイテム
#include "GameObject/StaticMeshObject/ItemBoxManager/ItemBoxType/ItemType.h"
#include "GameObject/StaticMeshObject/ItemBoxManager/ItemBox/CItemBox.h"

//ショットマネージャー
#include "GameObject/StaticMeshObject/Shot/ShotManager/CShotManager.h"	

//当たり判定.障害物判定用
#include "Collision/Collider/BoxCollider/CBoxCollider.h"

#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankBody/CBody.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankCannon/CCannon.h"

#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/CPlayer.h"

//ユーティリティクラス
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/Util/Util.h"


//-----ライブラリ-----
#include <d3dx9math.h>
#include <unordered_map>
#include <limits>
#include <unordered_set>
#include <memory>
#include <deque>
class CComPlayer
	: public CCharacterObjectBase
{
public:

	//オブジェクト
	struct SimpleObstacle
	{
		D3DXVECTOR3 pos;
		float radius;
	};

	CComPlayer();
	~CComPlayer() override;

	//動作関数.
	void Update() override;
	//描画関数.
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	//プレイヤーかCOMを判定する.
	bool IsPlayer() const override { return false; }

	//操作可能かどうか.
	void SetHasControl(bool enable) override { m_HasControl = enable; }
	bool HasControl() const override { return m_HasControl; }

	void Create(int id);

	static std::vector<CComPlayer*>& Instances();

	//追尾対象の設定
	void SetTarget(std::shared_ptr<CCharacterObjectBase> actor) { m_pTarget = std::move(actor); }
	void ClearTarget() { m_pTarget = nullptr; }


	//COMの有効無効を決める
	void SetComEnabled(bool enabled) { m_ComEnabled = enabled; }
	bool IsComEnabled() const { return m_ComEnabled; }

	//キャラクターマネージャーで使うよう
	void AttachShotManager(std::shared_ptr<CShotManager>& mgr) { m_pShotManager = mgr; }

	//プレイヤーを取得する.読み取り専用
	void SetPlayersRef(const std::vector<std::shared_ptr<CCharacterObjectBase>>* all) { m_pAllPlayer = all; }

	//マネージャーからアイテムの参照
	void SetItemBox(std::vector<std::shared_ptr<CItemBox>>* item) { m_pItemBox = item; }

	//障害物用のBOXセット
	void SetBoxColliders(const std::vector<std::shared_ptr<CBoxCollider>>* colliders)
	{
		m_pBoxCollider = colliders;
	}

	int GetPlayerID() const { return m_PlayerID; }

	void CreateCollider();

	//弾マネージャーの設定.
	void SetShotManager(std::shared_ptr<CShotManager> shot) override;

	D3DXVECTOR3 GetPosition() const override
	{
		if (m_pBody) return m_pBody->GetPosition();
		return D3DXVECTOR3(0, 0, 0);
	}

	//位置設定
	void SetPosition(const D3DXVECTOR3& pos) override
	{
		if (m_pBody)   m_pBody->SetPosition(pos);
		if (m_pCannon) m_pCannon->SetPosition(pos);
	}

	//回転取得
	D3DXVECTOR3 GetRotation() const override { if (m_pBody) return m_pBody->GetPosition(); return D3DXVECTOR3(0, 0, 0); }
	//回転設定
	void SetRotation(const D3DXVECTOR3& rot) override { if (m_pBody) return m_pBody->SetRotation(rot); }

	void FindNearestTarget();

	void SetSimpleObstacles(const std::vector<SimpleObstacle>* obstacles)
	{
		m_pSimpleObstacles = obstacles;
	}

	//キャラクターにヒットしたとき
	void CharacterHitRay();

private:
	//構造体
	//COMのショット関連のパラメータ
	struct ComShotState
	{
		int m_ShotCD = 0;						//クールダウン
		int	ShotCooldownFrames = 120;			//クールダウン時間
		float FireAngleEpsDeg = 360.f;			//この角度以内なら発射
		float MuzzleOffsetZ = 0.5f;				//砲口のオフセット
	};

	//列挙型
	//COMの状態
	enum class State
	{
		Seek,		//探索
		Chase,		//追跡
		Attack,		//攻撃
		Evade,		//離脱
	};

	//関数
	//フレームごとのステート処理
	void StepSeek();													//探索処理
	void StepChase();													//追跡処理
	void StepAttack();													//攻撃処理
	void StepEvade();													//離脱処理
	void StepItemSeek();												//アイテム探索処理
	void TryAutoFire();													//COMの弾発射処理
	void SanitizeParams();												//パラメータ調整
	void TickChaseTo(const D3DXVECTOR3& targetPos);						//追尾
	void TickAimTo(const D3DXVECTOR3& targetPos);						//砲塔追尾
	//void TickWander(float turnStep, float moveStep);
	void TickWander();													//引数なし
	void Blacklist(int id) { m_TargetBlackList[id] = m_BlackListTime; }	//一定時間ターゲットにしない
	bool IsBlacklisted(int id) const;									//IDがリストに登録されているか判定.読み取り専用
	void TickBlacklist();												//フレームごとにブラックリストを更新
	void SyncCannonToBody();											//砲塔を車体に追従させる
	void TransitionTo(State state);										//ステータスを変更する
	void EvaluateTransitions(float dist);								//条件に応じて状態変更	
	void MakeFixedTimeTarget();											//一定時間ターゲットにする
	void ComputeMuzzle(D3DXVECTOR3& outpos, float& outYaw) const;

	//障害物判定用
	bool SenseObstacleAABB(const CBoxCollider& selfBox, float yaw, D3DXVECTOR3& outAvoid, float& nearest) const;

	//前方に見えない当たり判定を置く
	bool HasObstacleAheadWithBox(const CBoxCollider& selfBox,
		const D3DXVECTOR3& forward,
		float probeDist,
		float step,
		float& outHitDist) const;

	//回避側に固定旋回を混ぜる
	float SteerWithAvoidAABB(float curYaw, float desiredYaw, float turnStep);

	bool FollowPath(float turnStep, float moveSte);

	//========================================
	// 安全な前進処
	//========================================
	void SafeAdvance(float nextYaw, float moveStep);

	//========================================
	// 危険ゾーン判定
	//========================================
	bool IsInDangerZone(const D3DXVECTOR3& pos) const;

	bool HasObstacleAheadSimple(const D3DXVECTOR3& selfPos, float yaw, float probeDist, float step, float& outHitDist) const;

	float NearestItemDist2(float& outDist2) const;

	//分離COMが重なったりするのを防ぐ計算
	void ComputeSeparation(const D3DXVECTOR3& selfPos,
		D3DXVECTOR3& outSep, float& outNearest) const;

	//COMの状態変更
	void ChangeState(State state);

	//外部クラス
	std::shared_ptr<CCharacterObjectBase> m_pTarget;							//追尾対象
	const std::vector<std::shared_ptr<CCharacterObjectBase>>* m_pAllPlayer;		//プレイヤーの一覧取得
	std::vector<std::shared_ptr<CItemBox>>* m_pItemBox;							//アイテムボックス
	std::weak_ptr<CItemBox> m_pItemTarget;										//弱参照のアイテムボックス
	const std::vector<std::shared_ptr<CBoxCollider>>* m_pBoxCollider;			//障害物のBoxColliderリスト
	std::unordered_set<const CCharacterObjectBase*> m_Black;
	const std::vector<SimpleObstacle>* m_pSimpleObstacles;						//障害物情報
	std::deque<D3DXVECTOR3> m_Path;												//ワールド座標WP列

	//COMの各パラメータ
	bool	m_ComEnabled;				//最初はCOM有効
	float	m_KeepDistance;				//この距離を保つ
	float	m_AvoidRadius;				//ほかCOMから離れる半径
	float	m_AvoidWeight;				//分離ベクトルの重み(0で無効.1強め)
	float	m_SeekRadius;				//視界外でもターゲットはManagerがくれるので大きめ
	float	m_AttacRadius;				//これ以内で攻撃モード
	float	m_FireConeDeg;				//砲塔の許容誤差
	float	m_ClosenessRadius;			//近くにしすぎないように一定に保つ半径
	int		m_EvadeDuration;			//回避するフレーム数
	int		m_EvadeFrames;
	D3DXVECTOR3 m_LastSeenPos;			//最後に見た位置
	int		m_LostSightFrames;
	bool	m_IsTarget;					//ターゲットかどうか	
	bool	m_Registered;				//インスタンス登録管理

	//探索処理パラメータ
	int		m_RetargetInterval;						//探索のインターバル
	int		m_RetargetTimer;						//カウント
	float	m_ForgetDistance;						//これ以上離れた忘れる
	float	m_StickinessRatio;						//既存ターゲット
	float	m_CurTargetDist;						//キャッシュ
	State	m_State;
	int		m_StateFrames;							//その状態に入ってからの経過フレーム
	float	m_WanderAngle;
	float	m_CurTargetDist2 = std::numeric_limits<float>::infinity();	// 現在ターゲットとの距離^2
	std::unordered_map<int, int> m_TargetBlackList;	//キーは相手のID.値は残りフレーム数
	int m_BlackListTime;							//何秒無視するか

	//アイテムが消えたので一旦消さずに放置
#if 0
	//アイテム
	int		m_RetargetItemTimer;		//アイテムタイマー
	int		m_RetargetItemInterval;		//アイテム探索インターバル 
	float	m_ItemGetRadius;			//範囲内なら狙う
	float	m_ItemPickUpRaius;			//以下なら取得.最終的には当たり判定でやる
	ComShotState m_ShotState;
#endif

	ComShotState m_ShotState;			//COMのショット情報

	//========================================
	// 障害物回避パラメータ
	//========================================
	//float		m_ProbeAngleRad;		// 探査角度（左右に振る角度）
	float		m_ProbeDist;			// 探査距離
	float		m_AvoidHoldFrames;		// 回避を維持するフレーム数
	int			m_AvoidSide;			// 回避方向（+1:右, -1:左, 0:未決定）
	float		m_AvoidMaxFrames;		// 最大回避フレーム
	float		m_BodyRadius;			// 自機の半径

	bool		m_Respawn;				// リスポーン

	D3DXVECTOR3 m_MapCenter;        // マップの中央座標（デフォルト 0,0,0）
	float m_WanderRadius;           // 中央からこの範囲内を徘徊（デフォルト 15.0）
	float m_CenterPullStrength;     // 中央への引き寄せ強度（デフォルト 0.3）

	float m_ObstacleProbeDist = 8.0f;								// 何メートル先まで見るか
	float m_ObstacleProbeStep = 0.1f;								// 何メートル刻みでチェックするか
	float m_ObstacleRadius = 1.5f;									// 自分の半径
	float m_ProbeAngleRad = D3DXToRadian(25.0f);					// 左右にどれくらい首を振るか
	float m_LookAheadSkep;

	//COMの複数敵判定
	float m_MultiEnemyRadius = 8.0f;	//この範囲内の敵をカウント
	float m_EscapeWeight = 0.6f;		//逃げの重み
	float m_ApproachWeight = 0.4f;		//攻めの重み
	int	  m_MultiEnemyThreshold = 2;	//この数以上で

	//レイがヒットしたかどうか
	bool m_RayHit;

	//複数体敵対応関数
	int CountNeardyEnemies(float radius, D3DXVECTOR3& outClusterCenter) const;

	float ComputeBlendedDirection(const D3DXVECTOR3& self,
		const D3DXVECTOR3& targetPos,
		const D3DXVECTOR3& clusterCenter,
		float escapeWeight,
		float approachWeight) const;


	//===ダメージの設定・取得===
	virtual void SetDamage(bool flg) override { m_Chara.m_Damage = flg; }
	virtual bool GetDamage() const override { return m_Chara.m_Damage; }
	//========================

	//=====死亡の設定・取得=====
	virtual void SetDeath(bool flg) override { m_Chara.m_Death = flg; }
	virtual bool GetDeath() const override { return m_Chara.m_Death; }
	//========================

	//=====無敵の設定・取得=====
	void SetMuteki(bool flg) override { m_Chara.m_Muteki = flg; }
	bool GetMuteki() const override { return m_Chara.m_Muteki; }
	//========================

	int GetPlayerID() override { return m_PlayerID; } 
};