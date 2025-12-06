#include "CComBrain.h"

// COM 共通キャラのベース
#include "GameObject/StaticMeshObject/Character/CharacterObject/CCharacterObject.h"

#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/CComUtility/CComUtility.h"

#include "GameObject/StaticMeshObject/ItemBoxManager/CItemBoxManager.h"

#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/CNavGrid/CNavGrid.h"

//====================
// ユーティリティ
//====================

#if 1
// [-π,π] に正規化
float CComBrain::Wrap(float a)
{
    const float TWO_PI = D3DX_PI * 2.0f;
    while (a > D3DX_PI) a -= TWO_PI;
    while (a < -D3DX_PI) a += TWO_PI;
    return a;
}

// 一方向に step だけ近づける
float CComBrain::Approach(float cur, float goal, float step)
{
    const float d = goal - cur;
    if (d > step) return cur + step;
    if (d < -step) return cur - step;
    return goal;
}
#endif


CComBrain::CComBrain()
    : m_Config                  ()
    , m_State                   ( ComCommand::State::Seek )
    , m_StateFrames             ( 0 )
    , m_LostSightFrames         ( 0 )
    , m_Target                  ()
    , m_pAllPlayer              ( nullptr )
    , m_BlackListTime           ( 120 )          
    , m_CurrentTargetDist2      ( 1e9f )
    , m_WanderAngle             ( 0.0f )
    , m_RetargetIntervalFrames  ( 120 )   // 2 秒ごとにターゲット見直し
    , m_RetargetTimer           ( 0 )
    , m_pUtility                ( nullptr )
{
    // Config のデフォルト値
    m_Config.keepDistance = 9.0f;
    m_Config.attackRadius = 10.0f;
    m_Config.seekRadius = 5.0f;
    m_Config.avoidRadius = 10.0f;
    m_Config.avoidWeight = 2.0f;
    m_Config.fireAngleEpsDeg = 10.0f;
    m_Config.forgetDistance = 60.0f;
    m_Config.stickinessRatio = 0.8f;
}

//狙う敵の判定
bool CComBrain::IsBlacklisted(int id) const
{
    auto it = m_TargetBlackList.find(id);
    return (it != m_TargetBlackList.end());
}

void CComBrain::Blacklist(int id)
{
    m_TargetBlackList[id] = m_BlackListTime;
}

//指定方向に障害物がないかチェック
bool CComBrain::IsDirectionSafe(const D3DXVECTOR3& from, float yaw, float checkDist) const
{
    if(!m_pNavGrid) return true;  // NavGridがなければ常に安全と見なす

    // 進行方向の先をチェック
    D3DXVECTOR3 dir(std::sinf(yaw), 0.0f, std::cosf(yaw));

    // 複数ポイントをチェック
    const float steps[] = { 2.0f, 4.0f, 6.0f };
    for (float d : steps)
    {
        if (d > checkDist) break;

        D3DXVECTOR3 checkPos = from + dir * d;
        GridPos gp = m_pNavGrid->WorldToGrid(checkPos);

        if (!m_pNavGrid->IsWalkable(gp))
        {
            return false;
        }
    }
    return true;
}

//ポジションの良さを評価
float CComBrain::EvaluatePosition(const D3DXVECTOR3& pos, const D3DXVECTOR3& targetPos) const
{
    float score = 0.0f;

    //ターゲットとの距離
    D3DXVECTOR3 toTarget = targetPos - pos;
    toTarget.y = 0;
    float dist = std::sqrtf(toTarget.x * toTarget.x + toTarget.z * toTarget.z);

    float distScore = 1.0f - std::fabs(dist - m_Config.keepDistance) / m_Config.keepDistance;
    distScore = std::max(0.0f, distScore);
    score += distScore * 50.0f;

    //通行可能かチェック
    if (m_pNavGrid)
    {
        GridPos gp = m_pNavGrid->WorldToGrid(pos);
        if (!m_pNavGrid->IsWalkable(gp))
        {
            return -1000.0f;  // 通行不可は最低スコア
        }

        //ターゲットへの視線が通るか
        GridPos targetGp = m_pNavGrid->WorldToGrid(targetPos);
        // 簡易的に中間点をチェック
        D3DXVECTOR3 mid = (pos + targetPos) * 0.5f;
        GridPos midGp = m_pNavGrid->WorldToGrid(mid);
        if (m_pNavGrid->IsWalkable(midGp))
        {
            score += 30.0f;  // 視線が通るならボーナス
        }
    }

    //ステージ中心からの距離
    float centerDist = std::sqrtf(pos.x * pos.x + pos.z * pos.z);
    if (centerDist > 20.0f)
    {
        score -= (centerDist - 20.0f) * 2.0f;
    }

    return score;
}

// Attack用: 最適な攻撃位置を探す
D3DXVECTOR3 CComBrain::FindBestAttackPosition(const D3DXVECTOR3& selfPos, const D3DXVECTOR3& targetPos) const
{
    D3DXVECTOR3 toTarget = targetPos - selfPos;
    toTarget.y = 0;
    float dist = std::sqrtf(toTarget.x * toTarget.x + toTarget.z * toTarget.z);

    if (dist < 0.1f) return selfPos;

    // ターゲットへの角度
    float toYaw = std::atan2f(toTarget.x, toTarget.z);

    // 8方向の候補位置を評価
    D3DXVECTOR3 bestPos = selfPos;
    float bestScore = -1e9f;

    const float checkAngles[] = {
        D3DX_PI * 0.5f,   // 左90度
        -D3DX_PI * 0.5f,  // 右90度
        D3DX_PI * 0.4f,   // 左72度
        -D3DX_PI * 0.4f,  // 右72度
        D3DX_PI * 0.6f,   // 左108度
        -D3DX_PI * 0.6f,  // 右108度
        0.0f,             // 直進（内側へ）
        D3DX_PI           // 後退（外側へ）
    };

    for (float angleOffset : checkAngles)
    {
        float moveYaw = Wrap(toYaw + angleOffset);
        D3DXVECTOR3 moveDir(std::sinf(moveYaw), 0.0f, std::cosf(moveYaw));

        // 少し先の位置を評価
        D3DXVECTOR3 candidatePos = selfPos + moveDir * 3.0f;

        float score = EvaluatePosition(candidatePos, targetPos);

        // 安全な方向かどうか
        if (IsDirectionSafe(selfPos, moveYaw, 5.0f))
        {
            score += 20.0f;
        }
        else
        {
            score -= 50.0f;  // 障害物方向はペナルティ
        }

        if (score > bestScore)
        {
            bestScore = score;
            bestPos = candidatePos;
        }
    }

    return bestPos;
}

D3DXVECTOR3 CComBrain::FindSafeEscapeDirection(const D3DXVECTOR3& selfPos, const D3DXVECTOR3& threatPos) const
{
    D3DXVECTOR3 away = selfPos - threatPos;
    away.y = 0;
    float len = std::sqrtf(away.x * away.x + away.z * away.z);

    if (len < 0.1f)
    {
        // 同じ位置なら適当に逃げる
        return D3DXVECTOR3(1.0f, 0.0f, 0.0f);
    }

    float awayYaw = std::atan2f(away.x, away.z);

    // 逃げる方向の候補
    const float candidates[] = {
        0.0f,                    // 真後ろ
        D3DX_PI * 0.25f,         // 右後方45度
        -D3DX_PI * 0.25f,        // 左後方45度
        D3DX_PI * 0.5f,          // 右横
        -D3DX_PI * 0.5f,         // 左横
        D3DX_PI * 0.75f,         // 右前方
        -D3DX_PI * 0.75f,        // 左前方
    };

    float bestYaw = awayYaw;
    float bestScore = -1e9f;

    for (float offset : candidates)
    {
        float testYaw = Wrap(awayYaw + offset);

        float score = 0.0f;

        // 安全な方向ならボーナス
        if (IsDirectionSafe(selfPos, testYaw, 8.0f))
        {
            score += 100.0f;
        }
        else
        {
            score -= 100.0f;
        }

        // 真後ろに近いほど良い
        score -= std::fabs(offset) * 10.0f;

        // ステージ中心に近づく方向を少し優遇
        D3DXVECTOR3 testDir(std::sinf(testYaw), 0.0f, std::cosf(testYaw));
        D3DXVECTOR3 newPos = selfPos + testDir * 5.0f;
        float newCenterDist = std::sqrtf(newPos.x * newPos.x + newPos.z * newPos.z);
        float curCenterDist = std::sqrtf(selfPos.x * selfPos.x + selfPos.z * selfPos.z);

        if (newCenterDist < curCenterDist)
        {
            score += 20.0f;  // 中心に近づくならボーナス
        }

        if (score > bestScore)
        {
            bestScore = score;
            bestYaw = testYaw;
        }
    }

    return D3DXVECTOR3(std::sinf(bestYaw), 0.0f, std::cosf(bestYaw));
}

float CComBrain::StabilizeDirection(float newYaw, float currentYaw)
{
    // クールダウン中は前回の決定を維持
    if (m_DecisionCooldown > 0)
    {
        m_DecisionCooldown--;
        return m_LastDecidedYaw;
    }

    // 新しい方向と前回の方向の差
    float diff = std::fabs(Wrap(newYaw - m_LastDecidedYaw));

    // 大きく変わる場合のみ更新（小さな変化は無視）
    const float CHANGE_THRESHOLD = D3DX_PI * 0.25f;  // 45度以上変わったら更新
    if (diff > CHANGE_THRESHOLD)
    {
        m_LastDecidedYaw = newYaw;
        m_DecisionCooldown = DECISION_INTERVAL;
    }

    return m_LastDecidedYaw;
}

void CComBrain::TickBlacklist()
{
    for (auto it = m_TargetBlackList.begin(); it != m_TargetBlackList.end();)
    {
        if (--(it->second) <= 0)
        {
            it = m_TargetBlackList.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

//====================
// ターゲット選択
//====================

// 一番近いターゲット
void CComBrain::UpdateTarget(const ComObservation& obs)
{
    //デバッグ出力
    printf("[UpdateTarget] selfID=%d, m_pAllPlayer=%p\n",
        obs.selfPlayerID, m_pAllPlayer);

    if (!m_pAllPlayer)
    {
        printf("  -> ERROR: m_pAllPlayer is nullptr!\n");
        m_Target.reset();
        m_CurrentTargetDist2 = 1e9f;
        return;
    }

    printf("  -> m_pAllPlayer size=%d\n", (int)m_pAllPlayer->size());

    const D3DXVECTOR3 selfPos = obs.selfPos;
    const int selfID = obs.selfPlayerID;

    std::shared_ptr<CCharacterObjectBase> best;
    float bestD2 = 1e9f;

    // 一番近いターゲット候補を探す
    int candidateCount = 0;
    for (auto& p : *m_pAllPlayer)
    {
        if (!p)
        {
            printf("  -> [%d] nullptr, skip\n", candidateCount);
            candidateCount++;
            continue;
        }

        const int pid = p->GetPlayerID();
        printf("  -> [%d] pid=%d", candidateCount, pid);

        if (pid == selfID)
        {
            printf(" (self, skip)\n");
            candidateCount++;
            continue;
        }
        if (IsBlacklisted(pid))
        {
            printf(" (blacklisted, skip)\n");
            candidateCount++;
            continue;
        }

        D3DXVECTOR3 pos = p->GetPosition();
        pos.y = 0.0f;
        D3DXVECTOR3 d = pos - selfPos;
        d.y = 0.0f;

        const float d2 = d.x * d.x + d.z * d.z;
        printf(" dist2=%.2f\n", d2);

        if (d2 < bestD2)
        {
            bestD2 = d2;
            best = p;
        }
        candidateCount++;
    }

    //結果を出力
    if (best)
    {
        printf("  -> Found target: pid=%d, dist2=%.2f\n",
            best->GetPlayerID(), bestD2);
    }
    else
    {
        printf("  -> No target found!\n");
    }

    // 候補がいないならターゲットなし
    if (!best)
    {
        m_Target.reset();
        m_CurrentTargetDist2 = 1e9f;
        return;
    }

    // すでにターゲットがいるならどれぐらい寄ってきたら乗り換えるかをstickinessで決める
    auto cur = m_Target.lock();
    if (!cur)
    {
        // まだターゲットがいない場合、そのまま採用
        m_Target = best;
        m_CurrentTargetDist2 = bestD2;
        printf("  -> Set new target: pid=%d\n", best->GetPlayerID());
        return;
    }

    // 現在ターゲットとの距離
    D3DXVECTOR3 cp = cur->GetPosition(); cp.y = 0.0f;
    D3DXVECTOR3 cd = cp - selfPos; cd.y = 0.0f;
    const float curD2 = cd.x * cd.x + cd.z * cd.z;

    // より十分近い敵が来たら乗り換え
    if (best.get() != cur.get() &&
        bestD2 < curD2 * m_Config.stickinessRatio)
    {
        m_Target = best;
        m_CurrentTargetDist2 = bestD2;
        printf("  -> Switched target to pid=%d\n", best->GetPlayerID());
    }
    else
    {
        m_CurrentTargetDist2 = curD2;
    }

    // 一定距離以上離れたら忘れてブラックリストに入れる
    const float forget2 = m_Config.forgetDistance * m_Config.forgetDistance;
    if (m_CurrentTargetDist2 > forget2)
    {
        printf("  -> Target too far, blacklisting pid=%d\n", cur->GetPlayerID());
        Blacklist(cur->GetPlayerID());
        m_Target.reset();
        m_CurrentTargetDist2 = 1e9f;
    }
}
//====================
// 状態遷移
//====================

void CComBrain::ChangeState(ComCommand::State s)
{
    m_State = s;
    m_StateFrames = 0;
    m_DecisionCooldown = 0;
}

// 状態を変える条件
void CComBrain::EvaluateTransitions(float dist2)
{
    const bool hasTarget = !m_Target.expired();

    //距離しきい値全部距離2で比較
    const float keep = std::max(m_Config.keepDistance, 1.0f);
    const float attackEnter2 = keep * 1.0f;                     // 攻撃モードに入る半径
    const float attackExit2 = keep * 1.6f;                      // 攻撃から離脱する半径
    const float evadeDist2 = keep * 0.4f;                       // かなり近づいたら退避
    const int   loseFrames = 120;                               // 2秒で見失い扱い

    const float attackEnter2Sq = attackEnter2 * attackEnter2;
    const float attackExit2Sq = attackExit2 * attackExit2;

    switch (m_State)
    {
    case ComCommand::State::Seek:
        if (hasTarget)
            ChangeState(ComCommand::State::Chase);
        break;

    case ComCommand::State::Chase:
        if (!hasTarget)
        {
            ChangeState(ComCommand::State::Seek);
            break;
        }
        if (dist2 <= evadeDist2)
        {
            ChangeState(ComCommand::State::Evade);
            break;
        }
        if (dist2 <= attackEnter2Sq)
        {
            ChangeState(ComCommand::State::Attack);
            break;
        }
        break;

    case ComCommand::State::Attack:
        if (!hasTarget)
        {
            ChangeState(ComCommand::State::Seek);
            break;
        }
        if (dist2 < evadeDist2)
        {
            ChangeState(ComCommand::State::Evade);
            break;
        }
        if (dist2 > attackExit2Sq)
        {
            ChangeState(ComCommand::State::Chase);
            break;
        }
        break;

    case ComCommand::State::Evade:
        if (!hasTarget)
        {
            ChangeState(ComCommand::State::Seek);
            break;
        }
        if (dist2 >= attackEnter2Sq)
        {
            ChangeState(ComCommand::State::Chase);
            break;
        }
        else if (dist2 >= evadeDist2)
        {
            ChangeState(ComCommand::State::Attack);
            break;
        }
        if (m_LostSightFrames > loseFrames)
        {
            ChangeState(ComCommand::State::Seek);
        }
        break;

    case ComCommand::State::ItemSeek:
        if (!hasTarget)
            ChangeState(ComCommand::State::Seek);
        break;
    }
}

//====================
// Wander 更新
//====================

void CComBrain::TickWander()
{
    const float WanderDelta = 0.10f;
    const float WanderClamp = 0.6f;

    //1/32 フレームくらいの頻度でランダムに向きを揺らす
    if ((std::rand() & 31) == 0)
    {
        const float sign = (std::rand() & 1) ? +1.0f : -1.0f;
        m_WanderAngle += sign * WanderDelta;
        if (m_WanderAngle > WanderClamp) m_WanderAngle = WanderClamp;
        if (m_WanderAngle < -WanderClamp) m_WanderAngle = -WanderClamp;
    }
}

//====================
// 各ステートのロジック
//====================

// Seek：ターゲットがいないとき適当にうろつく
void CComBrain::StepSeek(const ComObservation& obs, ComCommand& cmd)
{
    //ステージ中心
    const D3DXVECTOR3 center(0.f, 0.f, 0.f);

    D3DXVECTOR3 d = center - obs.selfPos;
    d.y = 0.0f;
    const float dist2 = d.x * d.x + d.z * d.z;

    float desiredYaw = obs.selfYaw;

    //ある程度外側にいたら中心に向かって進む
    const float centerRadius = 10.0f;           //この距離より外にいたら中心を優先
    if (dist2 > centerRadius * centerRadius)
    {
        desiredYaw = std::atan2f(d.x, d.z);
    }
    else
    {
        //中心付近ではWanderで探す
        TickWander();
        desiredYaw = obs.selfYaw + m_WanderAngle;
    }

    cmd.desiredBodyYaw = desiredYaw;
    cmd.moveStep = 1.0f;    
    cmd.aimAtTarget = false;
    cmd.tryFire = false;
}

// Attack：目標付近をぐるぐる回りながら攻撃
void CComBrain::StepAttack(const ComObservation& obs, ComCommand& cmd)
{
    auto target = m_Target.lock();
    if (!target)
    {
        ChangeState(ComCommand::State::Seek);
        cmd.state = ComCommand::State::Seek;
        return;
    }

    const D3DXVECTOR3 self = obs.selfPos;
    D3DXVECTOR3 tp = target->GetPosition();
    tp.y = 0.0f;

    const float dx = tp.x - self.x;
    const float dz = tp.z - self.z;
    const float dist = std::sqrtf(dx * dx + dz * dz);
    const float toYaw = std::atan2f(dx, dz);

    float desiredYaw;
    float moveSpeed = 1.0f;

    // 距離に応じた基本行動を決定
    const float keep = m_Config.keepDistance;

    if (dist > keep * 1.3f)
    {
        // 遠すぎ → 近づく
        desiredYaw = toYaw;

        // 直進が危険なら少しずらす
        if (!IsDirectionSafe(self, desiredYaw, 6.0f))
        {
            float leftYaw = Wrap(toYaw + 0.4f);
            float rightYaw = Wrap(toYaw - 0.4f);

            if (IsDirectionSafe(self, leftYaw, 6.0f))
                desiredYaw = leftYaw;
            else if (IsDirectionSafe(self, rightYaw, 6.0f))
                desiredYaw = rightYaw;
        }
    }
    else if (dist < keep * 0.6f)
    {
        // 近すぎ → 離れる
        desiredYaw = Wrap(toYaw + D3DX_PI);

        if (!IsDirectionSafe(self, desiredYaw, 5.0f))
        {
            // 後ろがダメなら横に
            float sideYaw = Wrap(toYaw + m_CircleDirection * (D3DX_PI * 0.5f));
            if (IsDirectionSafe(self, sideYaw, 5.0f))
                desiredYaw = sideYaw;
            else
                desiredYaw = Wrap(toYaw - m_CircleDirection * (D3DX_PI * 0.5f));
        }
    }
    else
    {
        // 適正距離 → 周回
        desiredYaw = Wrap(toYaw + m_CircleDirection * (D3DX_PI * 0.5f));

        // 周回方向が危険なら逆回り
        if (!IsDirectionSafe(self, desiredYaw, 6.0f))
        {
            m_CircleDirection = -m_CircleDirection;
            desiredYaw = Wrap(toYaw + m_CircleDirection * (D3DX_PI * 0.5f));

            // それでもダメならその場で射撃
            if (!IsDirectionSafe(self, desiredYaw, 6.0f))
            {
                desiredYaw = obs.selfYaw;  // 現在の向きを維持
                moveSpeed = 0.0f;          // 停止して射撃
            }
        }

        // 定期的に方向転換（フェイント）- ただし頻繁すぎない
        if (m_StateFrames > 0 && m_StateFrames % 120 == 0)  // 2秒ごと
        {
            float nextYaw = Wrap(toYaw - m_CircleDirection * (D3DX_PI * 0.5f));
            if (IsDirectionSafe(self, nextYaw, 6.0f))
            {
                m_CircleDirection = -m_CircleDirection;
            }
        }
    }

    //方向を安定化
    desiredYaw = StabilizeDirection(desiredYaw, obs.selfYaw);

    cmd.desiredBodyYaw = desiredYaw;
    cmd.moveStep = moveSpeed;
    cmd.aimAtTarget = true;
    cmd.tryFire = true;
    cmd.state = ComCommand::State::Attack;
}

// Evade：相手の逆方向へ逃げる
void CComBrain::StepEvade(const ComObservation& obs, ComCommand& cmd)
{
    auto target = m_Target.lock();
    if (!target)
    {
        ChangeState(ComCommand::State::Seek);
        cmd.state = ComCommand::State::Seek;
        return;
    }

    const D3DXVECTOR3 self = obs.selfPos;
    D3DXVECTOR3 tp = target->GetPosition();
    tp.y = 0.0f;

    // 基本は逆方向
    D3DXVECTOR3 away = self - tp;
    away.y = 0;
    float len = std::sqrtf(away.x * away.x + away.z * away.z);

    float desiredYaw;
    if (len < 0.1f)
    {
        desiredYaw = obs.selfYaw;
    }
    else
    {
        desiredYaw = std::atan2f(away.x, away.z);
    }

    // 逆方向が危険なら代替を探す
    if (!IsDirectionSafe(self, desiredYaw, 8.0f))
    {
        const float alternatives[] = { 0.5f, -0.5f, 1.0f, -1.0f };
        for (float offset : alternatives)
        {
            float testYaw = Wrap(desiredYaw + offset);
            if (IsDirectionSafe(self, testYaw, 8.0f))
            {
                desiredYaw = testYaw;
                break;
            }
        }
    }

    //方向を安定化
    desiredYaw = StabilizeDirection(desiredYaw, obs.selfYaw);

    cmd.desiredBodyYaw = desiredYaw;
    cmd.moveStep = 1.0f;
    cmd.aimAtTarget = true;
    cmd.tryFire = true;
    cmd.state = ComCommand::State::Evade;
}

// Chase障害物方向には突っ込まない
void CComBrain::StepChase(const ComObservation& obs, ComCommand& cmd)
{
    auto target = m_Target.lock();
    if (!target)
    {
        ChangeState(ComCommand::State::Seek);
        cmd.state = ComCommand::State::Seek;
        return;
    }

    const D3DXVECTOR3 self = obs.selfPos;
    D3DXVECTOR3 tp = target->GetPosition();
    tp.y = 0.0f;

    const float dx = tp.x - self.x;
    const float dz = tp.z - self.z;
    const float dist = std::sqrtf(dx * dx + dz * dz);

    float desiredYaw = std::atan2f(dx, dz);

    // 直進が危険なら少しずらす
    if (!IsDirectionSafe(self, desiredYaw, 8.0f))
    {
        const float offsets[] = { 0.3f, -0.3f, 0.6f, -0.6f };
        for (float off : offsets)
        {
            float testYaw = Wrap(desiredYaw + off);
            if (IsDirectionSafe(self, testYaw, 8.0f))
            {
                desiredYaw = testYaw;
                break;
            }
        }
    }

    // 方向を安定化（Chaseは比較的直線的なので緩め）
    // desiredYaw = StabilizeDirection(desiredYaw, obs.selfYaw);

    cmd.desiredBodyYaw = desiredYaw;

    float stepFactor = 1.0f;
    if (dist <= m_Config.keepDistance)
    {
        stepFactor = 0.0f;
    }
    cmd.moveStep = stepFactor;
    cmd.aimAtTarget = true;
    cmd.tryFire = true;
    cmd.state = ComCommand::State::Chase;
}//アイテムの探索・取得
void CComBrain::StepItemSeek(const ComObservation& obs, ComCommand& cmd)
{
#if 0
    if (!m_pAllItem)
    {
        m_Target.reset();
        return;
    }

    //自分のワールド座標
    const D3DXVECTOR3 selfPos = obs.selfPos;

    std::shared_ptr<CItemBox> best;
    float bestD2 = 1e9f;

    if (m_pItemBox->IsActive() == true)
    {
        //一番近いアイテムを探す
        for (auto& p : *m_pAllItem)
        {
            D3DXVECTOR3 pos = m_pItemBox->GetPosition();
            pos.y = 0.0f;
            D3DXVECTOR3 d = pos - selfPos;
            d.y = 0.0f;

            const float d2 = d.x * d.x + d.z * d.z;
            if (d2 < bestD2)
            {
                bestD2 = d2;
                best = p;
            }
        }
    } 

    //アイテムがなければターゲットもなし
    if (!best || m_pItemBox->IsActive() == false)
    {
        m_pItemBox.reset();
        m_CurrentTargetDist2 = 1e9;
    }

#if 0
    //ひとまずSeekと同じ挙動にしておく
    StepSeek(obs, cmd);
    cmd.state = ComCommand::State::ItemSeek;
#endif
#endif
}

void CComBrain::Update(const ComObservation& obs, ComCommand& outCmd)
{
    // コマンド初期化
    outCmd = ComCommand{};
    outCmd.state = m_State;

    auto t = m_Target.lock();
    printf("Brain: selfID=%d hasTarget=%d state=%d\n",
        obs.selfPlayerID,
        t ? 1 : 0,
        static_cast<int>(m_State));

    // ブラックリストの寿命更新
    TickBlacklist();

    // 一定間隔でターゲット再選択
    if (--m_RetargetTimer <= 0 || m_Target.expired())
    {
        UpdateTarget(obs);
        m_RetargetTimer = m_RetargetIntervalFrames;
    }

    // 現在ターゲットとの距離を計算
    float dist2 = 1e18f;
    if (auto t = m_Target.lock())
    {
        D3DXVECTOR3 tp = t->GetPosition();
        tp.y = 0.0f;
        D3DXVECTOR3 d = tp - obs.selfPos;
        d.y = 0.0f;
        dist2 = d.x * d.x + d.z * d.z;

        m_CurrentTargetDist2 = dist2;
        m_LostSightFrames = 0;
    }
    else
    {
        m_CurrentTargetDist2 = 1e18f;
        ++m_LostSightFrames;
    }

    // 状態遷移チェック
    EvaluateTransitions(dist2);

    // 状態ごとのロジックを実行
    switch (m_State)
    {
    case ComCommand::State::Seek:     StepSeek(obs, outCmd); break;
    case ComCommand::State::Chase:    StepChase(obs, outCmd); break;
    case ComCommand::State::Attack:   StepAttack(obs, outCmd); break;
    case ComCommand::State::Evade:    StepEvade(obs, outCmd); break;
    case ComCommand::State::ItemSeek: StepItemSeek(obs, outCmd); break;
    }

    outCmd.state = m_State;
    ++m_StateFrames;
}
