//-----継承するクラス-----
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//CPlayer.h" // プレイヤークラス

#include "GameObject/StaticMeshObject/Shot/ShotManager/CShotManager.h"	//ショットマネージャー

//-----外部のヘッダー-----
//アイテム
#include "GameObject/StaticMeshObject/ItemBoxManager/ItemBoxType/ItemType.h"
#include "GameObject/StaticMeshObject/ItemBoxManager/ItemBox/CItemBox.h"

//COM用の追尾クラス
#include "GameObject/StaticMeshObject/Character/COM/CChase/CChase.h"

//当たり判定.障害物判定用
#include "Collision/Collider/BoxCollider/CBoxCollider.h"

//COMデータ
#include "GameObject/StaticMeshObject/Character/COM/ComData/ComData.h"

//-----ライブラリ-----
#include <d3dx9math.h>
#include <unordered_map>
#include <limits>
#include <random>

//COMごとに別の動きを持たせるための列挙型
enum class ComStyle
{
	Aggressive,		//詰める
	StrafeLeft,		//左旋回
	StrafeLight,	//右旋回
	Sniper,			//遠距離
	Back,			//下がる
	forworld,		//前進
	Coward,			//回避系
	Collector,		//アイテム優先
};

enum class MovePolicy
{
	Straight,	//直進
	Left,		//左.回る
	light,		//右.回る

};


//
//enum class ComStyle {
//	Aggressive,   // グイグイ詰める
//	StrafeLeft,   // 左周りで周回
//	StrafeRight,  // 右周りで周回
//	Sniper,       // できるだけ止まって遠距離精度
//	Coward,       // すぐ下がる・回避重視
//	Collector,    // アイテム優先
//	Random        // ランダムに揺らす
//};
//
//enum class MovePolicy {
//	Straight,     // 直進（詰める）
//	OrbitL,       // 左回り周回（目標の周りを円運動）
//	OrbitR,       // 右回り周回
//	KeepAway,     // 離れる（距離を保つ／後退）
//	Hold          // その場維持
//};
//
//struct Personality {
//	float moveSpeedScale = 1.0f;  // 速度倍率
//	float turnSpeedScale = 1.0f;  // 旋回速度倍率
//	float keepDistance = 9.0f;  // 目標の維持距離
//	float avoidRadius = 10.0f; // 分離半径
//	float avoidWeight = 2.0f;  // 分離重み
//	float fireConeDeg = 10.0f; // 許容射角
//	int   retargetInterval = 120;   // リターゲット間隔
//	float stickinessRatio = 0.8f;  // 乗り換えにくさ
//	float wanderDelta = 0.08f; // Wander 角加算幅
//	float wanderClamp = 0.6f;  // Wander 角の上限
//	float strafeRadius = 9.0f;  // 周回するときの半径（=目標距離）
//	float strafeSpeedScale = 1.0f;  // 周回時の速度倍率
//	float keepAwayBias = 1.0f;  // 後退選好の強さ
//	float itemBias = 0.0f;  // アイテム志向（Collectorで大きめ）
//	float forgetDistance = 60.0f; // これ以上離れたら忘れる
//};
//
class CComPlayer
	: public CPlayer
{
public:
	//構造体
//COMのショット関連のパラメータ
	struct ComShotState
	{
		int m_ShotCD = 0;						//クールダウン
		int	ShotCooldownFrames = 120;			//クールダウン時間
		float FireAngleEpsDeg = 30;				//この角度以内なら発射
		float MuzzleOffsetZ = 1;				//砲口のオフセット
	};

public:
	CComPlayer();
	~CComPlayer() override;

	void Initialize(int id)override;
	void Update() override;

	//敵判定
	//自分以外は全員敵	
	bool IsEnemy(const CPlayer& other) const {return other.GetPlayerID() != m_PlayerID; };

	//追尾対象の設定
	void SetTarget(std::shared_ptr<CPlayer> player) { m_pTarget = player; }
	void ClearTarget() { m_pTarget = nullptr; }

	D3DXVECTOR3 GetPosition() const override;
	D3DXVECTOR3 GetRotation() const override;

	//COMの有効無効を決める
	void SetComEnabled(bool enabled) { m_ComEnabled = enabled; }
	bool IsComEnabled() const { return m_ComEnabled; }

	//プレイヤーマネージャーで使うよう
	void AttachShotManager(std::shared_ptr<CShotManager>& mgr) { m_pShotManager = mgr; }

	//プレイヤーを取得する.読み取り専用
	void SetPlayersRef(const std::vector<std::shared_ptr<CPlayer>>* all) { m_pAllPlayer = all; }

	//マネージャーからアイテムの参照
	void SetItemBox(std::vector<std::shared_ptr<CItemBox>>* item) { m_pItemBox = item; }

	//当たり判定用
	void SetObject(const std::vector<std::shared_ptr<CBoxCollider>>* BoxCollider) {};


	//ランダムで変更
	void Random(unsigned seed);

	//分離COMが重なったりするのを防ぐ計算
	void ComputeSeparation(const D3DXVECTOR3& selfPos,
		D3DXVECTOR3& outSep, float& outNearest) const;

	//COMインスタンスの静的レジストリ
	static std::vector<CComPlayer*>& Instances();


private:

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
	float  NearestItemDist2(float& outDist2) const;						//近い箱の距離2乗
	static float ToRad(float d) { return d * (D3DX_PI / 180.0f); }
	void ComputeMuzzle(D3DXVECTOR3& outpos, float& outYaw) const;

	void TickAimTo(const D3DXVECTOR3& targetPos);
	void TickChaseTo(const D3DXVECTOR3& target);

	//一方向にstepだけ近づける
	float Approach(float cur, float goal, float step)
	{
		const float d = goal - cur;
		if (d > step)  return cur + step;
		if (d < -step) return cur - step;
		return goal;
	}

	//[-π,π]に正規化
	float Wrap(float a)
	{
		while (a > PI())     a -= TWO_PI();
		while (a < -PI())     a += TWO_PI();
		return a;
	}

#if 0
	D3DXVECTOR3 ForwardFromYaw(float yaw)
	{
		return D3DXVECTOR3(std::sinf(yaw), 0.0f, std::cosf(yaw));
	}
#endif


	//COMのスタイル
	void ApplyStyle(ComStyle style);



	// ヘルパ
	static D3DXVECTOR3 ForwardFromYaw(float yaw) { return D3DXVECTOR3(std::sinf(yaw), 0.0f, std::cosf(yaw)); }      //(sin(yaw),0,cos(yaw))
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


	//COMの状態変更
	void ChangeState(State state);

	//外部クラス
	std::shared_ptr<CPlayer> m_pTarget;									//追尾対象
	std::weak_ptr<CShotManager> m_pShotManager;							//弾マネージャー.自動発射用のパラメータ
	const std::vector<std::shared_ptr<CPlayer>>* m_pAllPlayer;			//プレイヤーの一覧取得
	std::vector<std::shared_ptr<CItemBox>>* m_pItemBox;					//アイテムボックス
	std::weak_ptr<CItemBox> m_pItemTarget;								//弱参照のアイテムボックス
	const std::vector<std::shared_ptr<CBoxCollider>>* m_pBoxCollider;	//障害物の一部を外部から差し込む
	std::shared_ptr<ComData> m_pData;
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
};




