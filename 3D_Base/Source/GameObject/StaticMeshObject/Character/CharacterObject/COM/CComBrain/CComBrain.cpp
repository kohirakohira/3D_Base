#include "CComBrain.h"

// COM 共通キャラのベース
#include "GameObject/StaticMeshObject/Character/CharacterObject/CCharacterObject.h"

#include <algorithm>
#include <cmath>

//====================
// ユーティリティ
//====================

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

//====================
// コンストラクタ
//====================

CComBrain::CComBrain()
    : m_Config()
    , m_State(ComCommand::State::Seek)
    , m_StateFrames(0)
    , m_LostSightFrames(0)
    , m_Target()
    , m_pAllPlayer(nullptr)
    , m_BlackListTime(120)            // ざっくり 2 秒くらい無視 (60fps 前提)
    , m_CurrentTargetDist2(1e9f)
    , m_WanderAngle(0.0f)
    , m_RetargetIntervalFrames(120)   // 2 秒ごとにターゲット見直し
    , m_RetargetTimer(0)
{
    // Config のデフォルト値（必要ならここで上書き）
    m_Config.keepDistance = 9.0f;
    m_Config.attackRadius = 10.0f;
    m_Config.seekRadius = 5.0f;
    m_Config.avoidRadius = 10.0f;
    m_Config.avoidWeight = 2.0f;
    m_Config.fireAngleEpsDeg = 10.0f;
    m_Config.forgetDistance = 60.0f;
    m_Config.stickinessRatio = 0.8f;
}

//====================
// ブラックリスト系
//====================

bool CComBrain::IsBlacklisted(int id) const
{
    auto it = m_TargetBlackList.find(id);
    return (it != m_TargetBlackList.end());
}

void CComBrain::Blacklist(int id)
{
    m_TargetBlackList[id] = m_BlackListTime;
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
    if (!m_pAllPlayer)
    {
        m_Target.reset();
        m_CurrentTargetDist2 = 1e9f;
        return;
    }

    const D3DXVECTOR3 selfPos = obs.selfPos;
    const int selfID = obs.selfPlayerID;

    std::shared_ptr<CCharacterObjectBase> best;
    float bestD2 = 1e9f;

    //一番近いターゲット候補を探す
    for (auto& p : *m_pAllPlayer)
    {
        if (!p) continue;

        const int pid = p->GetPlayerID();
        if (pid == selfID) continue;
        if (IsBlacklisted(pid)) continue;

        D3DXVECTOR3 pos = p->GetPosition();
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

    //候補がいないならターゲットなし
    if (!best)
    {
        m_Target.reset();
        m_CurrentTargetDist2 = 1e9f;
        return;
    }

    //すでにターゲットがいるならどれぐらい寄ってきたら乗り換えるかをstickinessで決める
    auto cur = m_Target.lock();
    if (!cur)
    {
        // まだターゲットがいない場合、そのまま採用
        m_Target = best;
        m_CurrentTargetDist2 = bestD2;
        return;
    }

    //現在ターゲットとの距離
    D3DXVECTOR3 cp = cur->GetPosition(); cp.y = 0.0f;
    D3DXVECTOR3 cd = cp - selfPos; cd.y = 0.0f;
    const float curD2 = cd.x * cd.x + cd.z * cd.z;

    //より十分近い敵が来たら乗り換え
    if (best.get() != cur.get() &&
        bestD2 < curD2 * m_Config.stickinessRatio)
    {
        m_Target = best;
        m_CurrentTargetDist2 = bestD2;
    }
    else
    {
        m_CurrentTargetDist2 = curD2;
    }

    // 一定距離以上離れたら忘れてブラックリストに入れる
    const float forget2 = m_Config.forgetDistance * m_Config.forgetDistance;
    if (m_CurrentTargetDist2 > forget2)
    {
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
}

// 状態を変える条件（距離と見失いフレームだけを見る）
void CComBrain::EvaluateTransitions(float dist2)
{
    const bool hasTarget = !m_Target.expired();

    //距離しきい値全部距離2で比較
    const float keep = std::max(m_Config.keepDistance, 1.0f);
    const float attackEnter2 = std::max(keep * 1.05f, 3.0f);    // 攻撃モードに入る半径
    const float attackExit2 = std::max(keep * 1.25f, 5.0f);     // 攻撃から離脱する半径
    const float evadeDist2 = keep * keep * 0.60f * 0.60f;       // かなり近づいたら退避
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
        //今回は簡略化：ターゲットがいなければ探索へ戻る
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

    // 1/32 フレームくらいの頻度でランダムに向きを揺らす
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
    TickWander();

    const float cur = obs.selfYaw;
    const float desired = cur + m_WanderAngle;

    cmd.desiredBodyYaw = desired;
    cmd.moveStep = 1.0f; // 前進したい
    cmd.aimAtTarget = false;
    cmd.tryFire = false;
    cmd.state = ComCommand::State::Seek;
}

// Chase：ターゲットへ向かって詰める
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

    // 近づきすぎたときは少し横移動成分を混ぜる
    if (dist < m_Config.keepDistance * 0.9f)
    {
        const float side = ((m_StateFrames / 60) % 2 == 0) ? +1.0f : -1.0f;
        desiredYaw = Wrap(desiredYaw + side * (D3DX_PI * 0.5f));
    }

    cmd.desiredBodyYaw = desiredYaw;

    // 距離が十分なら 1.0、近すぎなら 0.0 （Body 側で moveSpeed を掛ける想定）
    float stepFactor = 1.0f;
    if (dist <= m_Config.keepDistance)
    {
        stepFactor = 0.0f;
    }
    cmd.moveStep = stepFactor;
    cmd.aimAtTarget = true;
    cmd.tryFire = true;
    cmd.state = ComCommand::State::Chase;
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

    // 基本は接線方向（左右どちらかに回る）
    const int   period = 60;
    const float sign = ((m_StateFrames / period) % 2 == 0) ? +1.0f : -1.0f;
    float desiredYaw = Wrap(toYaw + sign * (D3DX_PI * 0.5f));

    // 半径がズレたら少し修正
    const float keep = m_Config.keepDistance;
    if (dist > keep * 1.2f)
    {
        // 外に出過ぎた → 内側(ターゲット方向)へ
        desiredYaw = toYaw;
    }
    else if (dist < keep * 0.8f)
    {
        // 内側に入りすぎた → 少し外へ
        desiredYaw = Wrap(toYaw + D3DX_PI);
    }

    cmd.desiredBodyYaw = desiredYaw;
    cmd.moveStep = 1.0f;    // ぐるぐる回るので常に前進気味
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

    D3DXVECTOR3 away = self - tp;   // ターゲットから離れる方向
    away.y = 0.0f;

    const float len2 = away.x * away.x + away.z * away.z;
    if (len2 <= 1e-6f)
    {
        // 同一点に近すぎる場合は何もしないでその場で砲塔だけ回すなど
        cmd.desiredBodyYaw = obs.selfYaw;
        cmd.moveStep = 0.0f;
        cmd.aimAtTarget = true;
        cmd.tryFire = true;  // 逃げながら撃つなら true
        cmd.state = ComCommand::State::Evade;
        return;
    }

    const float desiredYaw = std::atan2f(away.x, away.z);

    cmd.desiredBodyYaw = desiredYaw;
    cmd.moveStep = 1.0f;    // しっかり逃げる
    cmd.aimAtTarget = true;    // 逃げながらもこちらを向いて撃ちたい
    cmd.tryFire = true;
    cmd.state = ComCommand::State::Evade;
}

// ItemSeek：今回はまだ簡略化しておく
void CComBrain::StepItemSeek(const ComObservation& obs, ComCommand& cmd)
{
    //ひとまずSeekと同じ挙動にしておく
    StepSeek(obs, cmd);
    cmd.state = ComCommand::State::ItemSeek;
}

//====================
// メイン Update
//====================

void CComBrain::Update(const ComObservation& obs, ComCommand& outCmd)
{
    // コマンド初期化
    outCmd = ComCommand{};
    outCmd.state = m_State;

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
