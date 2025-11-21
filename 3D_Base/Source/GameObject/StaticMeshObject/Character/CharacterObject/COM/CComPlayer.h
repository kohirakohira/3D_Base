//-----継承するクラス-----
#include "GameObject/StaticMeshObject/Character/CharacterObject/CCharacterObject.h"	//基底クラス.

//-----外部のヘッダー-----
//アイテム
#include "GameObject/StaticMeshObject/ItemBoxManager/ItemBoxType/ItemType.h"
#include "GameObject/StaticMeshObject/ItemBoxManager/ItemBox/CItemBox.h"

//ショットマネージャー
#include "GameObject/StaticMeshObject/Shot/ShotManager/CShotManager.h"	

//COM用の追尾クラス
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/CChase/CChase.h"

//当たり判定.障害物判定用
#include "Collision/Collider/BoxCollider/CBoxCollider.h"

#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankBody/CBody.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankCannon/CCannon.h"

#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/CPlayer.h"

//-----ライブラリ-----
#include <d3dx9math.h>
#include <unordered_map>
#include <limits>
#include <unordered_set>
#include <memory>


class CComPlayer
	: public CCharacterObjectBase
{
public:
	CComPlayer();
	~CComPlayer() override;

	void Create(int id)override;
	void Update() override;
	bool IsPlayer() const override { return false; }
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	// 位置の設定
	virtual void SetPosition(D3DXVECTOR3 pos) override {};
	// 位置を取得
	virtual const D3DXVECTOR3 GetPosition() override;

	// 回転の設定
	virtual void SetRotation(D3DXVECTOR3 rot) override {};
	// 回転を取得
	virtual const D3DXVECTOR3 GetRotation() override;

	// 拡縮の設定
	virtual void SetScale(D3DXVECTOR3 sca) override {};
	// 拡縮を取得
	virtual const D3DXVECTOR3 GetScale() override;

	static std::vector<CComPlayer*>& Instances();

	//リスポーンフラグの取得
	bool GetRespawnFlag() { return m_Respawn; }

	//リスポーンフラグの設定
	void SetRespawnFlag(bool flg) override {};

	//操作権の設定
	virtual void SetHasControl(bool control) override {};

	//車体の設定
	virtual void SetCBody(std::shared_ptr<CBody> pBody) override { m_pBody = pBody; }
	//車体の取得
	std::shared_ptr<CBody> GetBody() const override { return m_pBody; }

	//砲塔の設定
	virtual void SetCannon(std::shared_ptr<CCannon> pCannon) override { m_pCannon = pCannon; }
	//砲塔の取得
	std::shared_ptr<CCannon> GetCannon() const override { return m_pCannon; }

	
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

	//当たり判定用
	void SetObject(const std::vector<std::shared_ptr<CBoxCollider>>* BoxCollider) {};

	int GetPlayerID() const { return m_PlayerID; }

	// 戦車の座標、回転、拡縮を設定
	void SetTankPosition(const D3DXVECTOR3& pos) override {};
	void SetTankRotation(const D3DXVECTOR3& rot) override {};
	void SetTankScale(   const D3DXVECTOR3& sca) override {};

	//パラメータの設定.
	virtual void SetTuning(const TankTuning& tuning) override { m_Tuning = tuning; }
	//パラメータの取得.
	virtual const TankTuning& GetTuning() const override { return m_Tuning; }

private:
	//構造体
	//COMのショット関連のパラメータ
	struct ComShotState
	{
		int m_ShotCD = 0;						//クールダウン
		int	ShotCooldownFrames = 120;			//クールダウン時間
		float FireAngleEpsDeg = 30;				//この角度以内なら発射
		float MuzzleOffsetZ = 1;				//砲口のオフセット
	};

	//列挙型
	//COMの状態
	enum class State
	{
		Seek,		//探索
		Chase,		//追跡
		Attack,		//攻撃
		Evade,		//離脱
		ItemSeek,	//アイテム探索
	};

	//関数
	//フレームごとのステート処理
	void StepSeek();													//探索処理
	void StepChase();													//追跡処理
	void StepAttack();													//攻撃処理
	void StepEvade();													//離脱処理
	void StepItemSeek();												//アイテム探索処理
	void TryAutoFire();													//COMの弾発射処理
	void MakeItemTarget();												
	void SanitizeParams();												//パラメータ調整
	void TickChaseTo(const D3DXVECTOR3& targetPos);						//追尾
	void TickAimTo(const D3DXVECTOR3& targetPos);						//砲塔追尾
	void TickWander(float turnStep, float moveStep);
	void Blacklist(int id) { m_TargetBlackList[id] = m_BlackListTime; }	//一定時間ターゲットにしない
	bool IsBlacklisted(int id) const;									//IDがリストに登録されているか判定.読み取り専用
	void TickBlacklist();												//フレームごとにブラックリストを更新
	void SyncCannonToBody();											//砲塔を車体に追従させる
	void TransitionTo(State state);										//ステータスを変更する
	void EvaluateTransitions(float dist);								//条件に応じて状態変更	
	void MakeFixedTimeTarget();											//一定時間ターゲットにする
	static float Deg2Red(float d) { return d * (D3DX_PI / 180.0f); }
	static float DistXZ(const D3DXVECTOR3& a, const D3DXVECTOR3& b);
	static float AngleError(float fromYaw, const D3DXVECTOR3& fromPos, const D3DXVECTOR3& toPos);
	static float ToRad(float d) { return d * (D3DX_PI / 180.0f); }
	void ComputeMuzzle(D3DXVECTOR3& outpos, float& outYaw) const;

	//障害物判定用
	bool SenseObstacleAABB(const CBoxCollider& selfBox,float yaw,D3DXVECTOR3& outAvoid,float& nearest) const;

	//前方に見えない当たり判定を置く
	bool HasObstacleAheadWithBox(const CBoxCollider& selfBox,
		const D3DXVECTOR3& forward,
		float probeDist,
		float step,
		float& outHitDist) const;

	//回避側に固定旋回を混ぜる
	float SteerWithAvoidAABB(float curYaw, float desiredYaw, float turnStep);


	// ヘルパ
	static float Wrap(float rad);                         //[-π,π]に正規化
	static float Approach(float cur, float goal, float step);
	static D3DXVECTOR3 ForwardFromYaw(float yaw);         //(sin(yaw),0,cos(yaw))
	static float PI() { return D3DX_PI; }
	static float TWO_PI() { return D3DX_PI * 2.0f; }

	static inline float AngleDeadband(float a, float epsRad) {
		return (std::fabs(a) < epsRad) ? 0.0f : a;
	}
	static inline float ClampF(float v, float lo, float hi) {
		return (v < lo) ? lo : (v > hi) ? hi : v;
	}

	//動作切り替え
	static float Sqr(float v) { return v * v; }

	//分離COMが重なったりするのを防ぐ計算
	void ComputeSeparation(const D3DXVECTOR3& selfPos,
		D3DXVECTOR3& outSep, float& outNearest) const;

	//COMの状態変更
	void ChangeState(State state);

	//外部クラス
	std::shared_ptr<CCharacterObjectBase> m_pTarget;									//追尾対象
	std::weak_ptr<CShotManager> m_pShotManager;							//弾マネージャー.自動発射用のパラメータ
	const std::vector<std::shared_ptr<CCharacterObjectBase>>* m_pAllPlayer;			//プレイヤーの一覧取得
	std::vector<std::shared_ptr<CItemBox>>* m_pItemBox;					//アイテムボックス
	std::weak_ptr<CItemBox> m_pItemTarget;								//弱参照のアイテムボックス
	const std::vector<std::shared_ptr<CBoxCollider>>* m_pBoxCollider;	//障害物の一部を外部から差し込む
	std::unordered_set<const CCharacterObjectBase*> m_Black;

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

	//アイテム
	int		m_RetargetItemTimer;		//アイテムタイマー
	int		m_RetargetItemInterval;		//アイテム探索インターバル 
	float	m_ItemGetRadius;			//範囲内なら狙う
	float	m_ItemPickUpRaius;			//以下なら取得.最終的には当たり判定でやる
	ComShotState m_ShotState;

	const float m_ProdeAngleRad;		
	float		m_ProdeDist;
	float		m_AvoidHolde;
	int			m_AvoidSide;
	float		m_AvoidMax;

	int m_PlayerID = -1;

	bool		m_Respawn;				// リスポーン
};



