#include "CComPlayer.h"

#include <algorithm>    // std::remove
#include <cmath>

//==================================================
// 静的レジストリ
//==================================================
std::vector<CComPlayer*>& CComPlayer::Instances()
{
    static std::vector<CComPlayer*> s_list;
    return s_list;
}

//==================================================
// コンストラクタ / デストラクタ
//==================================================
CComPlayer::CComPlayer()
    : m_pBody                       ()
    , m_pCannon                     ()
    , m_pShotManager                ()
    , m_pAllPlayer                  ( nullptr )
    , m_pItemBox                    ( nullptr )
    , m_Brain                       ()
    , m_AvoidRadius                 ( 10.0f )
    , m_AvoidWeight                 ( 2.0f )
    , m_ObstacleRadius              ( 3.0f )
    , m_ObstacleProbeDist           ( 8.0f )
    , m_ObstacleProbeStep           ( 0.5f )
    , m_ProbeAngleRad(D3DXToRadian  ( 25.0f ))
    , m_pSimpleObstacles            ( nullptr )
    , m_Shot                        ()
    , m_ComEnabled                  ( true )
    , m_HasControl                  ( false )
    , m_Respawn                     ( false )
    , m_Registered                  ( false )
    , m_PlayerID                    ( -1 )
    , m_pUtility                   ( nullptr )
{
}

CComPlayer::~CComPlayer()
{
    if (m_Registered)
    {
        auto& list = Instances();
        list.erase(std::remove(list.begin(), list.end(), this), list.end());
        m_Registered = false;
    }
}

//==================================================
// 初期化
//==================================================
void CComPlayer::Initialize(int playerId)
{
    m_PlayerID = playerId;

    //車体と砲塔生成
    if (!m_pBody)   m_pBody = std::make_shared<CBody>(playerId);
    if (!m_pCannon) m_pCannon = std::make_shared<CCannon>(playerId);

    m_IsAlive = true;
    m_IsActive = true;
    m_Drawflag = true;

    SanitizeParams();

    if (!m_Registered)
    {
        Instances().push_back(this);
        m_Registered = true;
    }
}

//==================================================
// 外部参照の差し込み
//==================================================
void CComPlayer::SetPlayersRef(const std::vector<std::shared_ptr<CCharacterObjectBase>>* all)
{
    m_pAllPlayer = all;
    // Brain 側にも同じ配列を渡
    m_Brain.SetPlayersRef(all);
}

//==================================================
// パラメータの安全化
//==================================================
void CComPlayer::SanitizeParams()
{
    //auto& t = GetTuning();

    //if (t.moveSpeed <= 0.0f) t.moveSpeed = 0.05f;
    //if (t.bodyTurnSpeed <= 0.0f) t.bodyTurnSpeed = 0.03f;
    //if (t.turretTurnSpeed <= 0.0f) t.turretTurnSpeed = 0.03f;
    //if (t.cannonHeight <= 0.0f) t.cannonHeight = 0.5f;

    //if (m_AvoidRadius < 0.0f) m_AvoidRadius = 0.0f;
    //if (m_AvoidWeight < 0.0f) m_AvoidWeight = 0.0f;
    //if (m_ObstacleRadius <= 0.0f) m_ObstacleRadius = 3.0f;
    //if (m_ObstacleProbeDist <= 0.0f) m_ObstacleProbeDist = 8.0f;
    //if (m_ObstacleProbeStep <= 0.0f) m_ObstacleProbeStep = 0.5f;
    //if (m_ProbeAngleRad <= 0.0f) m_ProbeAngleRad = D3DXToRadian(25.0f);
}

#if 0
float CComPlayer::Wrap(float a)
{
    const float TWO_PI = D3DX_PI * 2.0f;
    while (a > D3DX_PI) a -= TWO_PI;
    while (a < -D3DX_PI) a += TWO_PI;
    return a;
}

float CComPlayer::Approach(float cur, float goal, float step)
{
    const float d = goal - cur;
    if (d > step) return cur + step;
    if (d < -step) return cur - step;
    return goal;
}
#endif

D3DXVECTOR3 CComPlayer::ForwardFromYaw(float yaw)
{
    return D3DXVECTOR3(std::sinf(yaw), 0.0f, std::cosf(yaw));
}

//==================================================
// CCharacterObjectBase インターフェース
//==================================================
D3DXVECTOR3 CComPlayer::GetCannonPosition() const
{
    if (m_pCannon) return m_pCannon->GetPosition();
    return D3DXVECTOR3(0, 0, 0);
}

float CComPlayer::GetCannonYaw() const
{
    if (m_pCannon) return m_pCannon->GetRotation().y;
    return 0.0f;
}

//==================================================
// Update / Draw
//==================================================
void CComPlayer::Update()
{
    SanitizeParams();

    // COM 無効時は見た目だけ更新
    if (!m_ComEnabled)
    {
        // ここはプロジェクトの実装に合わせて CStaticMeshObject::Update() などに変えてOK
        if (m_pBody)   m_pBody->Update();
        if (m_pCannon) m_pCannon->Update();
        return;
    }

    if (!m_pBody)
    {
        if (m_pCannon) m_pCannon->Update();
        return;
    }

    //観測値を作成して Brain に渡す
    ComObservation obs = BuildObservation();
    ComCommand     cmd;
    m_Brain.Update(obs, cmd);

    //指示を体の動きに反映
    ApplyCommand(cmd);
}

void CComPlayer::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
    if (!m_Drawflag) return;

    if (m_pBody)   m_pBody->Draw(View, Proj, Light, Camera);
    if (m_pCannon) m_pCannon->Draw(View, Proj, Light, Camera);
}

//ダメージ処理
void CComPlayer::OnHit(CCharacterObjectBase* )
{
    // HP 減少・リスポーン処理などがあればここに書く
}

//==================================================
// 観測値の組み立て
//==================================================
ComObservation CComPlayer::BuildObservation() const
{
    ComObservation obs{};

    if (m_pBody)
    {
        D3DXVECTOR3 pos = m_pBody->GetPosition();
        pos.y = 0.0f;
        obs.selfPos = pos;
        obs.selfYaw = m_pBody->GetRotation().y;
    }
    else
    {
        D3DXVECTOR3 pos = CCharacterObjectBase::GetPosition();
        pos.y = 0.0f;
        obs.selfPos = pos;
        obs.selfYaw = CCharacterObjectBase::GetRotation().y;
    }

    obs.selfPlayerID = m_PlayerID;
    return obs;
}

//==================================================
// Brain の指示を反映する
//==================================================
void CComPlayer::ApplyCommand(const ComCommand& cmd)
{
    auto body = GetBody();
    if (!body) return;

    const auto& t = GetTuning();

    const float curYaw = body->GetRotation().y;
    const float desiredYaw = cmd.desiredBodyYaw;

    // ステアリング
    const float nextYaw = SteerWithAvoid(curYaw, desiredYaw, t.bodyTurnSpeed);

    // 移動量（0～1想定）を実際の距離に変換
    float moveStep = std::max(0.0f, cmd.moveStep) * t.moveSpeed;

    // 安全に前進
    SafeAdvance(nextYaw, moveStep);

    // 砲塔の向き
    auto target = m_Brain.GetTarget().lock();   
    if (cmd.aimAtTarget && target)
    {
        AimTurretAt(target->GetPosition());
    }
    else
    {
        SyncCannonToBody();
    }

    // 発射
    if (cmd.tryFire && target)
    {
        TryAutoFire(cmd);
    }
}

//==================================================
// COM 同士の分離ベクトル
//==================================================
void CComPlayer::ComputeSeparation(const D3DXVECTOR3& selfPos,
    D3DXVECTOR3& outSep,
    float& outNearest) const
{
    outSep = D3DXVECTOR3(0, 0, 0);
    outNearest = 1e9f;

    if (m_AvoidRadius <= 0.0f) return;

    const float avoidR2 = m_AvoidRadius * m_AvoidRadius;

    for (CComPlayer* other : Instances())
    {
        if (other == this) continue;
        auto obBody = other->m_pBody;
        if (!obBody) continue;

        D3DXVECTOR3 offset = selfPos - obBody->GetPosition();
        offset.y = 0.0f;

        const float dist2 = offset.x * offset.x + offset.z * offset.z;
        if (dist2 <= 1e-6f)
        {
            outSep.x += 0.1f;
            continue;
        }

        outNearest = std::min(outNearest, std::sqrtf(dist2));

        if (dist2 < avoidR2)
        {
            const float inv = 1.0f / dist2;
            outSep.x += offset.x * inv;
            outSep.z += offset.z * inv;
        }
    }
}

//==================================================
// 障害物とのチェック
//==================================================
bool CComPlayer::HasObstacleAheadSimple(const D3DXVECTOR3& selfPos,
    float yaw,
    float probeDist,
    float step,
    float& outHitDist) const
{
    outHitDist = probeDist;
    if (!m_pSimpleObstacles || m_pSimpleObstacles->empty()) return false;

    const float selfR = m_ObstacleRadius;

    for (float d = step; d <= probeDist; d += step)
    {
        D3DXVECTOR3 p = selfPos + ForwardFromYaw(yaw) * d;
        p.y = 0.0f;

        for (const auto& ob : *m_pSimpleObstacles)
        {
            D3DXVECTOR3 v = p - ob.pos;
            v.y = 0.0f;
            const float dist2 = v.x * v.x + v.z * v.z;
            const float r = selfR + ob.radius;
            if (dist2 <= r * r)
            {
                outHitDist = d;
                return true;
            }
        }
    }
    return false;
}

//==================================================
// 危険ゾーン判定
//==================================================
bool CComPlayer::IsInDangerZone(const D3DXVECTOR3& pos) const
{
    if (!m_pSimpleObstacles) return false;

    const float selfR = m_ObstacleRadius;

    for (const auto& ob : *m_pSimpleObstacles)
    {
        const float dx = pos.x - ob.pos.x;
        const float dz = pos.z - ob.pos.z;
        const float r = selfR + ob.radius;
        if (dx * dx + dz * dz <= r * r)
            return true;
    }
    return false;
}

//==================================================
// ステアリング：障害物回避付き
//==================================================
float CComPlayer::SteerWithAvoid(float curYaw, float desiredYaw, float turnStep)
{
    auto body = m_pBody;
    if (!body) return curYaw;

    const D3DXVECTOR3 selfPos = body->GetPosition();

    //止まったらベースの角度をあげる
    const int STUCK_TURN_FRAMES = 30;   // 約0.5秒動けなかったら「スタック」とみなす
    float baseDesired = desiredYaw;
    if (m_StuckFrames >= STUCK_TURN_FRAMES)
    {
        // 奇数IDは左回り・偶数IDは右回りにしておくと、COM同士もばらける
        const float offset = ((m_PlayerID & 1) ? +1.0f : -1.0f) * D3DX_PI * 0.5f;
        baseDesired = m_pUtility->Wrap(desiredYaw + offset);
    }
    // ==============================================

    if (!m_pSimpleObstacles || m_pSimpleObstacles->empty())
    {
        const float d = m_pUtility->Wrap(baseDesired - curYaw);
        return m_pUtility->Approach(curYaw, curYaw + d, turnStep);
    }

    const float angs[3] = { 0.0f, +m_ProbeAngleRad, -m_ProbeAngleRad };

    float bestYaw = baseDesired;
    float bestScore = -1e9f;
    bool  anyFree = false;

    for (float a : angs)
    {
        const float testYaw = baseDesired + a;
        float hitD = m_ObstacleProbeDist;
        const bool blocked = HasObstacleAheadSimple(
            selfPos, testYaw,
            m_ObstacleProbeDist,
            m_ObstacleProbeStep,
            hitD);

        float score = 0.0f;
        if (!blocked)
        {
            score += 1000.0f;
            anyFree = true;
        }
        else
        {
            score -= (m_ObstacleProbeDist - hitD);
        }

        // なるべく baseDesired に近い方向を優先
        score -= std::fabs(m_pUtility->Wrap(testYaw - baseDesired)) * 10.0f;

        if (score > bestScore)
        {
            bestScore = score;
            bestYaw = testYaw;
        }
    }

    // 3候補すべて障害物で塞がれている場合の最終手段
    if (!anyFree && m_StuckFrames > 0)
    {
        const float offset = ((m_PlayerID & 1) ? +1.0f : -1.0f) * D3DX_PI * 0.5f;
        bestYaw = m_pUtility->Wrap(curYaw + offset);
    }

    const float d = m_pUtility->Wrap(bestYaw - curYaw);
    return m_pUtility->Approach(curYaw, curYaw + d, turnStep);
}

//==================================================
// 安全な前進
//==================================================
void CComPlayer::SafeAdvance(float nextYaw, float moveStep)
{
    auto body = GetBody();
    if (!body) return;

    D3DXVECTOR3 pos = body->GetPosition();
    pos.y = 0.0f;

    // COM 同士の分離
    D3DXVECTOR3 sep(0, 0, 0);
    float nearest = 1e9f;
    ComputeSeparation(pos, sep, nearest);

    D3DXVECTOR3 nextPos = pos + ForwardFromYaw(nextYaw) * moveStep;

    if (sep.x != 0.0f || sep.z != 0.0f)
    {
        const float len = std::sqrtf(sep.x * sep.x + sep.z * sep.z);
        if (len > 1e-6f)
        {
            sep.x /= len; sep.z /= len;
            nextPos.x += sep.x * m_AvoidWeight * 0.1f;
            nextPos.z += sep.z * m_AvoidWeight * 0.1f;
        }
    }
    nextPos.y = 0.0f;

    const bool wantsMove = (moveStep > 0.01f);

    if (IsInDangerZone(nextPos))
    {
        // 前に出ると危険なので、その場で向きだけ変える
        body->SetRotation(D3DXVECTOR3(0.0f, nextYaw, 0.0f));
        body->Update();
        SyncCannonToBody();

        // 動きたかったのに動けなかった → スタックカウント
        if (wantsMove)
        {
            ++m_StuckFrames;
        }
        else
        {
            m_StuckFrames = 0;
        }
        return;
    }

    // 実際に移動できたのでスタック解除
    m_StuckFrames = 0;

    body->SetRotation(D3DXVECTOR3(0.0f, nextYaw, 0.0f));
    body->SetPosition(nextPos);
    body->Update();
    SyncCannonToBody();
}

//==================================================
// 砲塔関連
//==================================================
void CComPlayer::SyncCannonToBody()
{
    if (!m_pCannon) return;

    const auto& t = GetTuning();
    D3DXVECTOR3 base;

    if (m_pBody) base = m_pBody->GetPosition();
    else         base = m_pCannon->GetPosition();

    base.y += t.cannonHeight;
    m_pCannon->SetPosition(base);
}

void CComPlayer::AimTurretAt(const D3DXVECTOR3& targetPos)
{
    auto cannon = m_pCannon;
    if (!cannon) return;

    const auto& t = GetTuning();

    D3DXVECTOR3 base = m_pBody ? m_pBody->GetPosition() : cannon->GetPosition();
    base.y += t.cannonHeight;

    D3DXVECTOR3 to = targetPos - base;
    to.y = 0.0f;
    if (to.x == 0.0f && to.z == 0.0f)
    {
        SyncCannonToBody();
        return;
    }

    const float desired = std::atan2f(to.x, to.z);
    float yaw = cannon->GetRotation().y;
    yaw = m_pUtility->Approach(yaw, yaw + m_pUtility->Wrap(desired - yaw), t.turretTurnSpeed);

    cannon->SetPosition(base);
    cannon->SetRotation(D3DXVECTOR3(0.0f, yaw, 0.0f));
    cannon->Update();
}

void CComPlayer::ComputeMuzzle(D3DXVECTOR3& outPos, float& outYaw) const
{
    outPos = D3DXVECTOR3(0, 0, 0);
    outYaw = 0.0f;

    auto body = m_pBody;
    auto cannon = m_pCannon;
    const auto& t = GetTuning();

    D3DXVECTOR3 base(0, 0, 0);
    float yaw = 0.0f;

    if (body)
    {
        base = body->GetPosition();
        yaw = body->GetRotation().y;
    }
    if (cannon)
    {
        if (!body) base = cannon->GetPosition();
        yaw = cannon->GetRotation().y;
    }

    base.y += t.cannonHeight;
    const D3DXVECTOR3 fwd = ForwardFromYaw(yaw);

    outPos = base + fwd * m_Shot.muzzleOffsetZ;
    outYaw = yaw;
}

//==================================================
// ショット
//==================================================
void CComPlayer::TryAutoFire(const ComCommand&)
{
    auto manager = m_pShotManager.lock();
    if (!manager) return;

    auto target = m_Brain.GetTarget().lock();
    if (!target) return;

    // クールダウン中ならまだ撃たない
    if (m_Shot.coolDownFrames > 0)
    {
        --m_Shot.coolDownFrames;
        return;
    }

    D3DXVECTOR3 muzzle; float yaw;
    ComputeMuzzle(muzzle, yaw);

    // 砲口からターゲットへの方向
    D3DXVECTOR3 to = target->GetPosition() - muzzle;
    to.y = 0.0f;
    const float d2 = to.x * to.x + to.z * to.z;
    if (d2 <= 1e-6f) return;

    const float desired = std::atan2f(to.x, to.z);
    const float err = std::fabs(m_pUtility->Wrap(desired - yaw));

    // 砲塔の向きがある程度合っていたら撃つ
    const float maxErr = D3DXToRadian(m_Shot.fireAngleEpsDeg);   
    if (err <= maxErr)
    {
     /*   manager->(
            m_PlayerID,
            muzzle,
            yaw
        );*/

        m_Shot.coolDownFrames = m_Shot.maxCoolDown;
    }
}

