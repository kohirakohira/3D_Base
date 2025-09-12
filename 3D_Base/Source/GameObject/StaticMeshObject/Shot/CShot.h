#pragma once
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" /* 継承クラス || スタティックメッシュオブジェクトクラス */

/**************************************************
*	弾クラス(単発)
* ShotManagerがプレイヤーごとに複数個所持する
**/
class CShot
	: public CStaticMeshObject
{
public:
	CShot();
	virtual ~CShot() override;

	void Initialize(int id);
	virtual void Update() override;
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	//弾を再設定
	void Reload(const D3DXVECTOR3& Pos, float RotY);

	void SetDisplay(bool disp) { m_Display = disp; }
	bool IsActive() const { return m_Display; }

private:
	bool		m_Display = false;		// 表示切替
	D3DXVECTOR3 m_MoveDirection;		// 移動方向
	float		m_MoveSpeed= 0.5f;		// 移動速度
	int 		m_LifeFramesInit = 120;	// 約3秒くらい表示する
	int			m_LifeFrames = 120;		// フレームカウンタ
	float 		m_Gravity = 1.0f;		// 弾の重力
	float 		m_VelocityY = 0.0f;		// 加速度

#if 0
protected:
	struct Shot
	{
		bool		m_Display			= false;				// 表示切替
		D3DXVECTOR3 m_MoveDirection;							// 移動方向
		float		m_MoveSpeed			= 0.002f;				// 移動速度　※とりあえず0.2fを設定;		// 移動速度
		int			m_DisplayTime;								// 約3秒くらい表示する
		float		m_Gravity			= 0.001f;				// 弾の重力
		float		m_Velocity			= 0.001f;				// 加速度

	};

private:
	Shot	m_Shot[ShotMax];
#endif
};
