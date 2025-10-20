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
	void Update() override;
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	//弾を再設定
	void Reload(const D3DXVECTOR3& Pos, float RotY);

	void SetDisplay(bool disp) { m_Shot.m_Display = disp; }
	bool IsActive() const;

protected:
	//弾構造体.
	struct Shot
	{
		bool		m_Display;				// 表示切替
		D3DXVECTOR3 m_MoveDirection;		// 移動方向
		float		m_MoveSpeed;			// 移動速度
		int			m_DisplayTime;			// 約3秒くらい表示する
		float		m_Gravity;				// 弾の重力
		float		m_Velocity;				// 加速度
	};

private:
	//変数宣言.
	Shot	m_Shot;
};
