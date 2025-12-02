#pragma once

//COMの体クラス

class IComBody
{
public:

	virtual ~IComBody() = default;

	//読み取り専用な情報
	virtual D3DXVECTOR3 GetPos() const = 0;
	virtual float GetYaw() const = 0;
	virtual float GetTurretYaw() const = 0;

    //速度パラメータ
    virtual float MoveSpeed()      const = 0;
    virtual float BodyTurnSpeed()  const = 0;
    virtual float TurretTurnSpeed()const = 0;
    virtual float CannonHeight()   const = 0;

    //操作コマンド
    virtual void RotateBodyToward(float desiredYaw, float maxDelta) = 0;
    virtual void MoveForward(float step) = 0;

    virtual void AimTurretAt(const D3DXVECTOR3& targetPos,
        float maxDelta) = 0;

    virtual void TryFireAt(const D3DXVECTOR3& targetPos)
};