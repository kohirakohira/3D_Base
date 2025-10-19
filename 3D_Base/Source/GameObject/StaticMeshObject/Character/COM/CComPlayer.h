//-----継承するクラス-----
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//CPlayer.h" // プレイヤークラス

#include "GameObject/StaticMeshObject/Shot/ShotManager/CShotManager.h"	//ショットマネージャー

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
	std::shared_ptr<CPlayer> m_Target;	//追尾対象
	bool m_Registered;	//インスタンス登録管理

	//自動発射用のパラメータ
	std::weak_ptr<CShotManager> m_pShotManager; //弾マネージャー
	int m_ShotCD;				//クールダウン
	int	ShotCooldownFrames;		//クールダウン時間(フレーム)
	float FireAngleEpsDeg;		//この角度以内なら発射
	float MuzzleOffsetZ;		//砲口のオフセット


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

	bool m_ComEnabled = true;	//最初はCOM有効

	//float m_MoveSpeed;			//1フレームの前進量
	//float m_TurnStep;				//1フレームの回頭量
	//float m_AimTurnStep;			//砲塔回頭の1フレーム量
	float m_KeepDistance;			//この距離を保つ
	//float m_CannonHeight;			//砲塔の高さオフセッ
	float m_AvoidRadius;			//ほかCOMから離れる半径
	float m_AvoidWeight;			//分離ベクトルの重み(0で無効.1強め)


private:
	//COMの状態
	enum class State
	{
		Idle,	//待機
		Seek,	//探索
		Chase,	//追跡
		Attac,	//攻撃
		Evade,	//回避,離脱
	};
	State m_State = State::Idle;
	int m_StateFrames;			//その状態に入ってからの経過フレーム

	void ChangeState(State state)
	{
		m_State = state;
		m_StateFrames = 0;
	}

	//フレームごとのステート処理
#if 1
	void StepIdle();
	void StepSeek();
	void StepChase();
	void StepAttack();
	void StepEvade();
#endif

	//COMの弾発射処理
	void TryAutoFire();
#if 0
	// 判定ヘルパ
	float DistXZ(const D3DXVECTOR3& a, const D3DXVECTOR3& b) const;
	bool  HasTarget() const { return (bool)m_Target; }
	bool  InSight(const D3DXVECTOR3& self, const D3DXVECTOR3& tgt) const;
	bool  InAttackCone(float yaw, const D3DXVECTOR3& self, const D3DXVECTOR3& tgt, float epsRad) const;
	bool  ShouldEvade(float nearest) const; // 近接COMとの距離で判定
};
#endif
};

