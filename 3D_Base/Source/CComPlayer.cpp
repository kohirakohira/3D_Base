#include "CComPlayer.h"
#include "CBody.h"
#include "CCannon.h"
#include <cmath>

#undef min

// ---- 小ヘルパ ----
static inline float PI() { return D3DX_PI; }
static inline float TWO_PI() { return D3DX_PI * 2.0f; }

//静的レジストリ.複数をいっきに扱う
std::vector<CComPlayer*>& CComPlayer::Instances() {
    static std::vector<CComPlayer*> registry;
    return registry;
}

CComPlayer::CComPlayer()
    : MoveSpeed(0.10f)     // 見やすい初期値
    , TurnStep(0.08f)
    , AimTurnStep(0.12f)
    , KeepDistance(9.0f)   // 0ならベタ詰め
    , CannonHeight(0.3f)
    , m_Target(nullptr)
    , m_AvoidRadius ( 3.0f )
    , m_AvoidWeight ( 0.8f )
    , m_Registered ( false )
{
}

//そのCOM自身を削除する処理
CComPlayer::~CComPlayer()
{
    if (m_Registered) {
        auto& comList = Instances();
        //comListのなかからthisを探してremoveで後ろに回す、無効かされたのをまとめて削除
        comList.erase(std::remove(comList.begin(), comList.end(), this), comList.end());
        m_Registered = false;
    }
}

void CComPlayer::Initialize(int id)
{
    CPlayer::Initialize(id);
    SanitizeParams();

    //自分がまだ登録されていなければ、全体リストに登録する
    if (!m_Registered) {
        Instances().push_back(this);
        m_Registered = true;
    }
}

void CComPlayer::SetTarget(std::shared_ptr<CPlayer> player)
{
    m_Target = player;
}
void CComPlayer::ClearTarget()
{
    m_Target = nullptr;
}

D3DXVECTOR3 CComPlayer::GetPosition() const
{
    if (m_pBody)return m_pBody->GetPosition();
    return CCharacter::GetPosition();
}

D3DXVECTOR3 CComPlayer::GetRotation() const
{
    if (m_pBody) return m_pBody->GetRotation();
    return CCharacter::GetRotation();
}

//不正値を防ぐ
void CComPlayer::SanitizeParams()
{
    if (MoveSpeed <= 0.0f)      MoveSpeed       = 0.06f;
    if (TurnStep <= 0.0f)       TurnStep        = 0.08f;
    if (AimTurnStep <= 0.0f)    AimTurnStep     = 0.12f;
    if (CannonHeight == 0.0f)   CannonHeight    = 0.3f;
    if (KeepDistance < 0.0f)    KeepDistance    = 0.0f;
    if (m_AvoidRadius < 0.0f)   m_AvoidRadius   = 0.0f;
    if (m_AvoidWeight < 0.0f)   m_AvoidWeight = 0.0f;
}

// [-π,π]に正規化
float CComPlayer::Wrap(float a)
{
    while (a > PI())     a -= TWO_PI();
    while (a < -PI())     a += TWO_PI();
    return a;
}

// 一方向にstepだけ近づける（過不足なく）
float CComPlayer::Approach(float cur, float goal, float step)
{
    const float d = goal - cur;
    if (d > step)  return cur + step;
    if (d < -step) return cur - step;
    return goal;
}

D3DXVECTOR3 CComPlayer::ForwardFromYaw(float yaw)
{
    return D3DXVECTOR3(std::sinf(yaw), 0.0f, std::cosf(yaw));
}

//COM同士の分離ベクトルを計算
void CComPlayer::ComputeSeparation(const D3DXVECTOR3& selfPos,
        D3DXVECTOR3& outSep, float& outNearest)const
{
    outSep = D3DXVECTOR3(0, 0, 0);
    outNearest = 1e9f;  //大きい値.fをつけてるのはfloat型にするから
       
    if (m_AvoidRadius <= 0.0f)return;   //回避半径が0以下なら何もしない

    const float avoidRadius = m_AvoidRadius;
    const float avoidRadiusSq = avoidRadius * avoidRadius;

    for (CComPlayer* other : Instances()) {
        if (other == this) continue;
        std::shared_ptr<CBody> ob = other ? other->Body() : nullptr;
        if (!ob)continue;   //位置が取れない相手は無視する

        D3DXVECTOR3 offset = selfPos - ob->GetPosition();
        offset.y = 0.0f; //高さは無視

        const float distSq = offset.x * offset.x + offset.z * offset.z;
        if (distSq <= 1e-6f) {
            //ほぼ同一点のため少し押す
            outSep.x += 0.1f;
            continue;
        }
        //一番近い相手までの距離を更新
        outNearest = std::min(outNearest, std::sqrtf(distSq));

        if (distSq < avoidRadiusSq) {
            //近いほど強い反発
            const float invDistSq = 1.0f / distSq;
            outSep.x += offset.x * invDistSq;
            outSep.z += offset.z * invDistSq;
        }
    }
    //正規化は呼び出し側でブレンド時にやる
}

// 本体を常にターゲットへ回頭＋前進
void CComPlayer::TickChaseTo(const D3DXVECTOR3& targetPos)
{
    std::shared_ptr<CBody> body = Body();
    if (!body) return;

    // 現在姿勢
    D3DXVECTOR3 pos = body->GetPosition();
    float yaw = body->GetRotation().y;

    // 水平面の差分
    D3DXVECTOR3 to = targetPos - pos; to.y = 0.0f;
    const float d2 = to.x * to.x + to.z * to.z;

    // 目標方位へ最短差で回頭
    if (d2 > 1e-6f) {
        const float desiredYaw = std::atan2f(to.x, to.z);               // +Z前 左手座標
        yaw = Approach(yaw, yaw + Wrap(desiredYaw - yaw), TurnStep);
    }

    D3DXVECTOR3 chaseDir(0, 0, 0);
    if (d2 > 1e-6f) {
        const float inv = 1.0f / std::sqrtf(d2);
        chaseDir.x = to.x * inv;    //正規化
        chaseDir.z = to.z * inv;
    }

    //分離ベクトル
    D3DXVECTOR3 sep(0, 0, 0);
    float nearest = 1e9f;
    ComputeSeparation(pos, sep, nearest);

    //ブレンド
    D3DXVECTOR3 desire = chaseDir;  //目標優先
    if (m_AvoidWeight > 0.0f && (sep.x != 0.0f || sep.z != 0.0f)) {
        //sepを正規化して重み付け
        const float sepLen = std::sqrt(sep.x * sep.x + sep.z * sep.z);
        if (sepLen > 1e-6f) {
            sep.x /= sepLen; sep.z /= sepLen;
            desire.x += sep.x * m_AvoidWeight;
            desire.z += sep.z * m_AvoidWeight;
        }
    }

    //合成方向が有効ならその方位にいく
    float desiredYaw = yaw;
    const float desLen2 = desire.x * desire.x + desire.z * desire.z;
    if (desLen2 > 1e-8f) {
        desiredYaw = std::atan2f(desire.x, desire.z);
        yaw = Approach(yaw, yaw + Wrap(desiredYaw - yaw), TurnStep);
    }
 
    //前進量の決定
    float step = MoveSpeed;
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
    if (nearest < 1e9f && m_AvoidRadius > 0.0f) {
        float scale = nearest / m_AvoidRadius;
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
}

// 砲塔があれば常にターゲットを向く
void CComPlayer::TickAimTo(const D3DXVECTOR3& targetPos)
{
    std::shared_ptr<CCannon> cannon = Cannon();
    const std::shared_ptr<CBody> body = Body();
    if (!cannon) return;

    // 砲塔の基準位置
    D3DXVECTOR3 base = body ? body->GetPosition() : cannon->GetPosition();
    base.y += CannonHeight;

    // 目標方位
    const D3DXVECTOR3 to = targetPos - base;
    const float desiredYaw = std::atan2f(to.x, to.z);

    float cyaw = cannon->GetRotation().y;
    cyaw = Approach(cyaw, cyaw + Wrap(desiredYaw - cyaw), AimTurnStep);

    cannon->SetPosition(base);
    cannon->SetRotation(D3DXVECTOR3(0.0f, cyaw, 0.0f));
    cannon->CCharacter::Update();
}

void CComPlayer::Update()
{
    SanitizeParams();

    // ターゲット不在でも見た目は更新
    std::shared_ptr<CBody> body = Body();
    if (!body) { if (auto c = Cannon()) c->CCharacter::Update(); return; }

    // 追尾対象がなければ回頭も移動もせず、そのまま更新
    if (!m_Target) {
        body->CCharacter::Update();
        if (auto c = Cannon()) c->CCharacter::Update();
        return;
    }

    // 自己ターゲットは無視
    if (m_Target.get() == this) {
        body->CCharacter::Update();
        if (auto c = Cannon()) c->CCharacter::Update();
        return;
    }


    const D3DXVECTOR3 tp = m_Target->GetPosition();
    TickChaseTo(tp);
    TickAimTo(tp);
}
