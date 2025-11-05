#include "ComData.h"
#include "GameObject/StaticMeshObject/Character/COM/CComPlayer.h"   


// 本体を常にターゲットへ回頭＋前進
void ComData::TickChaseTo(const D3DXVECTOR3& targetPos, float AvoidWeight, float KeepDistance, float AvoidRadius)
{
#if 1
    auto com = std::shared_ptr<CComPlayer>();

    //パラメータ取得
    auto& tuning = GetTuning();

    std::shared_ptr<CBody> body = Body();
    if (!body) return;

    // 現在姿勢
    D3DXVECTOR3 pos = body->GetPosition();
    float yaw = body->GetRotation().y;

    // 水平面の差分
    D3DXVECTOR3 target = targetPos - pos;
    target.y = 0.0f;
    const float d2 = target.x * target.x + target.z * target.z;

    // 目標方位へ最短差で回頭
    if (d2 > 1e-6f) {
        const float desiredYaw = std::atan2f(target.x, target.z);               // +Z前 左手座標
        yaw = Approach(yaw, yaw + Wrap(desiredYaw - yaw), tuning.turretTurnSpeed);
    }

    D3DXVECTOR3 chaseDir(0, 0, 0);
    if (d2 > 1e-6f) {
        const float inv = 1.0f / std::sqrtf(d2);
        chaseDir.x = target.x * inv;    //正規化
        chaseDir.z = target.z * inv;
    }

    //分離ベクトル
    D3DXVECTOR3 sep(0, 0, 0);
    float nearest = 1e9f;
    com->ComputeSeparation(pos, sep, nearest);

    //ブレンド
    D3DXVECTOR3 desire = chaseDir;  //目標優先
    if (AvoidWeight > 0.0f && (sep.x != 0.0f || sep.z != 0.0f)) {
        //sepを正規化して重み付け
        const float sepLen = std::sqrt(sep.x * sep.x + sep.z * sep.z);
        if (sepLen > 1e-6f) {
            sep.x /= sepLen; sep.z /= sepLen;
            desire.x += sep.x * AvoidWeight;
            desire.z += sep.z * AvoidWeight;
        }
    }

    //合成方向が有効ならその方位にいく
    float desiredYaw = yaw;
    const float desLen2 = desire.x * desire.x + desire.z * desire.z;
    if (desLen2 > 1e-8f) {
        desiredYaw = std::atan2f(desire.x, desire.z);
        yaw = Approach(yaw, yaw + Wrap(desiredYaw - yaw), tuning.turretTurnSpeed);
    }
 
    //前進量の決定
    float step = tuning.moveSpeed;
    if (d2 > 0.0f) {
        const float dist = std::sqrtf(d2);
        if (KeepDistance > 0.0f) {
            const float remain = dist - KeepDistance;
            if (remain <= 0.0f) {
                step = 0.0f;    //これ以上は詰めない
            }
            else if (step > remain) {
                step = remain;
            }
            else
            {
                if (step > dist)step = dist;
            }
        }
    }

    //COMが近すぎるときは減速.完全停止もする
    if (nearest < 1e9f &&  AvoidRadius > 0.0f) {
        float scale = nearest / AvoidRadius;
        if (scale < 0.0f) scale = 0.0f;
        if (scale > 1.0f) scale = 1.0f;
        step *= scale;
    }

    // 前進（ヨーに沿って +Z 基準で）
    if (step > 0.0f) {
        const D3DXVECTOR3 fwd = ForwardFromYaw(yaw);
        pos += fwd * step;
    }

    // 反映
    body->SetRotation(D3DXVECTOR3(0.0f, yaw, 0.0f));
    body->SetPosition(pos);
    body->CCharacter::Update();
#endif
}

// 砲塔があれば常にターゲットを向く
void ComData::TickAimTo(const D3DXVECTOR3& targetPos)
{
#if 1
    auto& tuning = GetTuning();
    std::shared_ptr<CCannon> cannon = Cannon();
    const std::shared_ptr<CBody> body = Body();
    if (!cannon) return;

    // 砲塔の基準位置
    D3DXVECTOR3 base = body ? body->GetPosition() : cannon->GetPosition();
    base.y += tuning.cannonHeight;

    // 目標方位
    const D3DXVECTOR3 target = targetPos - base;
    const float desiredYaw = std::atan2f(target.x, target.z);

    float cyaw = cannon->GetRotation().y;
    cyaw = Approach(cyaw, cyaw + Wrap(desiredYaw - cyaw), tuning.turretTurnSpeed);

    cannon->SetPosition(base);
    cannon->SetRotation(D3DXVECTOR3(0.0f, cyaw, 0.0f));
    cannon->CCharacter::Update();
#endif

}

