#include "CComObstacleAvoidance.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/Util/Util.h"
#include <cmath>

CComObstacleAvoidance::CComObstacleAvoidance()
    : m_pObstacles      (nullptr)
    , m_SelfRadius      (1.0f)
    , m_ProbeDist       (5.0f)
    , m_ProbeStep       (0.5f)
    , m_ProbeAngleRad   (0.785f)
{
}

bool CComObstacleAvoidance::HasObstacleAhead(
    const D3DXVECTOR3& selfPos,
    float yaw,
    float probeDist,
    float step,
    float& outHitDist) const
{
    outHitDist = probeDist;

    // 障害物リストのチェック
    if (!m_pObstacles || m_pObstacles->empty())
    {
        return false;
    }

    // 前方を少しずつスキャンするループ
    for (float d = step; d <= probeDist; d += step)
    {
        // yawから前方向の正規化ベクトルを返す
        D3DXVECTOR3 p = selfPos + Util::ForwardFromYaw(yaw) * d;
        p.y = 0.0f;  // X.Z平面だけで判断

        // 障害物リストを1つずつ見る
        for (const auto& ob : *m_pObstacles)
        {
            D3DXVECTOR3 v = p - ob.pos;
            v.y = 0.0f;
            float dist2 = v.x * v.x + v.z * v.z;
            float r = m_SelfRadius + ob.radius;

            if (dist2 <= r * r)
            {
                outHitDist = d;
                return true;
            }
        }
    }
    return false;
}

bool CComObstacleAvoidance::IsInDangerZone(const D3DXVECTOR3& pos) const
{
    if (!m_pObstacles)
    {
        return false;
    }

    for (const auto& ob : *m_pObstacles)
    {
        float dx = pos.x - ob.pos.x;
        float dz = pos.z - ob.pos.z;
        float r = ob.radius + m_SelfRadius;

        if (dx * dx + dz * dz < r * r)
        {
            return true;
        }
    }
    return false;
}

float CComObstacleAvoidance::ComputeAvoidYaw(
    const D3DXVECTOR3& selfPos,
    float curYaw,
    float desiredYaw,
    float turnStep) const
{
    // 障害物がない場合
    if (!m_pObstacles || m_pObstacles->empty())
    {
        float d = Util::Wrap(desiredYaw - curYaw);
        return Util::Approach(curYaw, curYaw + d, turnStep);
    }

    // 目標方向に対して試すオフセット角度のリスト
    const float angs[3] = { 0.f, +m_ProbeAngleRad, -m_ProbeAngleRad };

    float bestYaw = desiredYaw;
    float bestScore = -1e9f;

    for (float a : angs)
    {
        float testYaw = desiredYaw + a;
        float hitD;
        bool blocked = HasObstacleAhead(selfPos, testYaw, m_ProbeDist, m_ProbeStep, hitD);
        float score = 0.0f;

        if (!blocked)
        {
            score += 1000.0f;
        }
        else
        {
            score -= (m_ProbeDist - hitD);
        }

        score -= std::fabs(Util::Wrap(testYaw - desiredYaw)) * 10.0f;

        if (score > bestScore)
        {
            bestScore = score;
            bestYaw = testYaw;
        }
    }

    float d = Util::Wrap(bestYaw - curYaw);
    return Util::Approach(curYaw, curYaw + d, turnStep);
}