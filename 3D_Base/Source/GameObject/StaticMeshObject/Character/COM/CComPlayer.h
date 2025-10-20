//-----継承するクラス-----
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//CPlayer.h" // プレイヤークラス

#include "GameObject/StaticMeshObject/Shot/ShotManager/CShotManager.h"	//ショットマネージャー

//-----外部のヘッダー-----
//アイテム
#include "GameObject/StaticMeshObject/ItemBoxManager/ItemBoxType/ItemType.h"
#include "GameObject/StaticMeshObject/ItemBoxManager/ItemBox/CItemBox.h"

//-----ライブラリ-----
#include <d3dx9math.h>



class CComPlayer
	: public CPlayer
{
public:
	CComPlayer();
	~CComPlayer() override;

	void Initialize(int id)override;
	void Update() override;

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


private:
	std::shared_ptr<CPlayer> m_pTarget;		//追尾対象
	std::shared_ptr<CItemBox> m_pItemBox;	//アイテムボックスクラス
	bool m_Registered;	//インスタンス登録管理

	//自動発射用のパラメータ
	std::weak_ptr<CShotManager> m_pShotManager; //弾マネージャー


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

	//COMの各パラメータ
	bool	m_ComEnabled;				//最初はCOM有効
	float	m_KeepDistance;				//この距離を保つ
	float	m_AvoidRadius;				//ほかCOMから離れる半径
	float	m_AvoidWeight;				//分離ベクトルの重み(0で無効.1強め)
	float	m_SeekRadius;				//この範囲ないなら
	float	m_AttacRadius;				//どの範囲から攻撃体制に入るか
	float	m_FireConeDeg;				//砲塔の許容誤差
	float	m_ClosenessRadius;			//近くにしすぎないように一定に保つ半径
	int		m_EvadeDuration;			//回避するフレーム数
	int		m_EvadeFrames;


	//COMのショット関連のパラメータ
	struct ComShotState
	{
		int m_ShotCD = 60;					//クールダウン
		int	ShotCooldownFrames = 60;		//クールダウン時間(フレーム)
		float FireAngleEpsDeg;				//この角度以内なら発射
		float MuzzleOffsetZ;				//砲口のオフセット
	};
	ComShotState m_ShotState;

	//COMの状態
	enum class State
	{
		Idle,		//待機
		Seek,		//探索
		Chase,		//追跡
		Attack,		//攻撃
		Evade,		//離脱
		ItemSeek,	//アイテム探索
	};
	State m_State = State::Idle;
	int m_StateFrames;			//その状態に入ってからの経過フレーム
//	int   m_EvadeFrames = 0;
	void ChangeState(State state)
	{
		m_State = state;
		m_StateFrames = 0;
	}

	//フレームごとのステート処理
	void StepIdle();		//待機処理
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

	
	void TransitionTo(State state);
};

//一旦退避.初期化は消した


