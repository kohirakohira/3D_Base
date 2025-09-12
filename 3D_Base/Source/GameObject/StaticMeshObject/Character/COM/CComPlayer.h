//-----継承するクラス-----
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//CPlayer.h" // プレイヤークラス

//-----ライブラリ-----
#include <d3dx9math.h>

//-----外部クラス-----
#include "GameObject/StaticMeshObject/Shot/ShotManager/CShotManager.h"	//ショットマネージャー
#include "GameObject/StaticMeshObject/Character/CCharacter.h"			//キャラクタークラス.レイ用

class CComPlayer 
	: public CPlayer
{
public:

	enum class ComPattern
	{
		Idle,			//停止
		Priority = 0,	//優先度
		Aim,			//狙う
		ItemSearch,		//アイテム探索
		PlayerSearch,	//プレイヤー探索.COMも含めて

	};

	//操作できる対象のタイプ
	enum class ActiveType
	{
		player,
		com
	};

	CComPlayer();
	~CComPlayer() override;

	void Initialize(int id)override;
	void Update() override;

	//追尾対象の設定
	void SetTarget(std::shared_ptr<CPlayer> player);
	void ClearTarget();

	//チューニング値,m_を付け忘れたのであとで修正する
	float MoveSpeed;			// 1フレームの前進量
	float TurnStep;				// 1フレームの回頭量
	float AimTurnStep;			// 砲塔回頭の1フレーム量
	float KeepDistance;			// この距離を保つ
	float CannonHeight;			// 砲塔の高さオフセッ
	float m_AvoidRadius;		// ほかCOMから離れる半径
	float m_AvoidWeight;		// 分離ベクトルの重み(0で無効.1強め)

	D3DXVECTOR3 GetPosition() const override;
	D3DXVECTOR3 GetRotation() const override;

	void AttachShotManager(std::shared_ptr<CShotManager>& mgr) { m_pShotManager = mgr; }

	void SetActiveType(ActiveType ActiveType) { m_ActiveType = ActiveType; }
	ActiveType GetActiveType() const { return m_ActiveType; }

private:
	ComPattern m_ComPattern;			//COMの行動パターン
	ActiveType m_ActiveType;			//操作できる対象
	int m_Priority = 0;					//優先度
	std::shared_ptr<CPlayer> m_Target;	//追尾対象
	bool m_Registered;					//インスタンス登録管理

	//自動発射用のパラメータ
	std::weak_ptr<CShotManager> m_pShotManager; //弾マネージャー
	int m_ShotCD;				//クールダウン
	int	ShotCooldownFrames;		//クールダウン時間(フレーム)
	float FireAngleEpsDeg;		//この角度以内なら発射
	float MuzzleOffsetZ;		//砲口のオフセット

private:
	//内部処理
	void SanitizeParams();
	void TickChaseTo(const D3DXVECTOR3& targetPos);
	void TickAimTo(const D3DXVECTOR3& targetPos);

	// ヘルパ
	static float Wrap(float rad);                         // [-π,π]に正規化
	static float Approach(float cur, float goal, float step);
	static D3DXVECTOR3 ForwardFromYaw(float yaw);         // (sin(yaw),0,cos(yaw))

	//分離COMが重なったりするのを防ぐ計算
	void ComputeSeparation(const D3DXVECTOR3& selfPos,
		D3DXVECTOR3& outSep, float& outNearest) const;

	//COMインスタンスの静的レジストリ
	static std::vector<CComPlayer*>& Instances();

	static inline float ToRad(float d) { return d * (D3DX_PI / 180.0f); }

	void ComputeMuzzle(D3DXVECTOR3& outpos, float& outYaw) const;

};