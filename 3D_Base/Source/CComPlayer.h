#include "CPlayer.h"
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

	//チューニング値,m_を付け忘れたのであとで修正する
	float MoveSpeed;		// 1フレームの前進量
	float TurnStep;			// 1フレームの回頭量
	float AimTurnStep;		// 砲塔回頭の1フレーム量
	float KeepDistance;		// この距離を保つ
	float CannonHeight;		// 砲塔の高さオフセッ
	float m_AvoidRadius;		// ほかCOMから離れる半径
	float m_AvoidWeight;		// 分離ベクトルの重み(0で無効.1強め)

	D3DXVECTOR3 GetPosition() const override;
	D3DXVECTOR3 GetRotation() const override;

private:
	std::shared_ptr<CPlayer> m_Target;	//追尾対象
	bool m_Registered;	//インスタンス登録管理

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

};