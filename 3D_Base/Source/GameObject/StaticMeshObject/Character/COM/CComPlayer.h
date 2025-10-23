//-----継承するクラス-----
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//CPlayer.h" // プレイヤークラス

#include "GameObject/StaticMeshObject/Shot/ShotManager/CShotManager.h"	//ショットマネージャー

//-----外部のヘッダー-----
//アイテム
#include "GameObject/StaticMeshObject/ItemBoxManager/ItemBoxType/ItemType.h"
#include "GameObject/StaticMeshObject/ItemBoxManager/ItemBox/CItemBox.h"

//-----ライブラリ-----
#include <d3dx9math.h>
#include <unordered_map>
#include <limits>


class CComPlayer
	: public CPlayer
{
public:
	CComPlayer();
	~CComPlayer() override;

	void Initialize(int id)override;
	void Update() override;

	//敵判定
	inline bool IsEnemy(const CPlayer& other) const;

	//追尾対象の設定
	void SetTarget(std::shared_ptr<CPlayer> player);
	void ClearTarget();

	D3DXVECTOR3 GetPosition() const override;
	D3DXVECTOR3 GetRotation() const override;

	//COMの有効無効を決める
	void SetComEnabled(bool enabled) { m_ComEnabled = enabled; }
	bool IsComEnabled() const { return m_ComEnabled; }

	//プレイヤーマネージャーで使うよう
	void AttachShotManager(std::shared_ptr<CShotManager>& mgr) { m_pShotManager = mgr; }

	//プレイヤーを取得する.読み取り専用
	void SetPlayersRef(const std::vector<std::shared_ptr<CPlayer>>* all);

#if 0
	// マネージャから全アイテム配列の参照（生存期間は外側で管理）
	void SetItemBoxesRef(std::vector<std::shared_ptr<CItemBox>>* boxes) { m_pItemBoxes = boxes; }

#endif

private:
	std::shared_ptr<CPlayer> m_pTarget;		//追尾対象
	bool m_Registered;	//インスタンス登録管理

	//自動発射用のパラメータ
	std::weak_ptr<CShotManager> m_pShotManager; //弾マネージャー

	//プレイヤーの一覧取得
	const std::vector<std::shared_ptr<CPlayer>>* m_pAllPlayer;

	//内部処理
	void SanitizeParams();
	void TickChaseTo(const D3DXVECTOR3& targetPos);
	void TickAimTo(const D3DXVECTOR3& targetPos);

	static inline float ToRad(float d) { return d * (D3DX_PI / 180.0f); }
	void ComputeMuzzle(D3DXVECTOR3& outpos, float& outYaw) const;


	// ヘルパ
	static float Wrap(float rad);                         // [-π,π]に正規化
	static float Approach(float cur, float goal, float step);
	static D3DXVECTOR3 ForwardFromYaw(float yaw);         // (sin(yaw),0,cos(yaw))

	//分離COMが重なったりするのを防ぐ計算
	void ComputeSeparation(const D3DXVECTOR3& selfPos,
		D3DXVECTOR3& outSep, float& outNearest) const;

	//COMインスタンスの静的レジストリ
	static std::vector<CComPlayer*>& Instances();

	//COMの状態
	enum class State
	{
		Seek,		//探索
		Chase,		//追跡
		Attack,		//攻撃
		Evade,		//離脱
		ItemSeek,	//アイテム探索
	};


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

	//探索処理パラメータ
	int		m_RetargetInterval;					//探索のインターバル
	int		m_RetargetTimer;					//カウント
	float	m_ForgetDistance;					//これ以上離れた忘れる
	float	m_StickinessRatio;					//既存ターゲット
	float	m_CurTargetDist;					//キャッシュ
	State	m_State;
	int m_StateFrames;							//その状態に入ってからの経過フレーム
	float m_WanderAngle = 0.0f;
	float m_CurTargetDist2 = std::numeric_limits<float>::infinity(); // 現在ターゲットとの距離^2

	std::unordered_map<int, int> m_TargetBlackList;	//キーは相手のID.値は残りフレーム数
	int m_BlackListTime = 1200;						//何秒無視するか

	void TickWander(float turnStep, float moveStep);

	//一定時間ターゲットにしない
	void Blacklist(int id) { m_TargetBlackList[id] = m_BlackListTime;
	}

	//IDがリストに登録されているか判定
	//読み取り専用
	bool IsBlacklisted(int id) const
	{
		auto it = m_TargetBlackList.find(id);
		return it != m_TargetBlackList.end();
	}
	//フレームごとにブラックリストを更新
	void TickBlacklist()
	{
		for (auto it = m_TargetBlackList.begin(); it != m_TargetBlackList.end();)
		{
			if (--(it->second) <= 0) {	//残りフレーム数を減らす
				it = m_TargetBlackList.erase(it);	//0以下ならその要素を削除して次のア
			}
			else {
				++it;	//まだ残っているなら次
			}
		}
	}

	//仮の宣言
	float	m_TargetRadius;				//ターゲット扱いする距離


	//COMのショット関連のパラメータ
	struct ComShotState
	{
		int m_ShotCD = 0;						//クールダウン
		int	ShotCooldownFrames = 120;			//クールダウン時間
		float FireAngleEpsDeg = 30;				//この角度以内なら発射
		float MuzzleOffsetZ = 1;				//砲口のオフセット
	};
	ComShotState m_ShotState;

	//	int   m_EvadeFrames = 0;
	void ChangeState(State state)
	{
		m_State = state;
		m_StateFrames = 0;
	}

	//フレームごとのステート処理
	void StepSeek();		//探索処理
	void StepChase();		//追跡処理
	void StepAttack();		//攻撃処理
	void StepEvade();		//離脱処理
	void StepItemSeek();	//アイテム探索処理

	//COMの弾発射処理
	void TryAutoFire();

	//ToRed使う
	static inline float Deg2Red(float d) { return d * (D3DX_PI / 180.0f); }

	static inline float DistXZ(const D3DXVECTOR3& a, const D3DXVECTOR3& b);

	static inline float AngleError(float fromYaw, const D3DXVECTOR3& fromPos, const D3DXVECTOR3& toPos);

	//砲塔を車体に追従させる
	void SyncCannonToBody();

	//ステータスを変更する
	void TransitionTo(State state);

	//条件に応じて状態変更
	void EvaluateTransitions(float dist);

	//一定時間ターゲットにする
	void MakeFixedTimeTarget();

	void TickOrbit(const D3DXVECTOR3& targetPos, float r, float angStep);

#if 0
	// アイテムターゲティング用
	std::vector<std::shared_ptr<CItemBox>>* m_pItemBoxes = nullptr;
	std::weak_ptr<CItemBox> m_pItemTarget;

	int   m_ItemRetargetTimer = 0;
	int   m_ItemRetargetInterval = 30;		// 0.5秒ごと
	float m_ItemAttractRadius = 20.f;		// この距離以内なら狙う
	float m_ItemPickupRadius = 1.2f;		// これ以下とれた

	void   MakeItemTarget();      // アクティブ箱を選定
	float  NearestItemDist2(float& outDist2) const; // 近い箱の距離^2
#endif
};



