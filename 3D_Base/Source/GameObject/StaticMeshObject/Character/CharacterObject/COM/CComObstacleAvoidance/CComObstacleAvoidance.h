#pragma once
#include <d3dx9math.h>
#include <vector>

// áŠQ•¨î•ñ
struct SimpleObstacle
{
    D3DXVECTOR3 pos;
    float radius;
};

/*
    COM‚ÌáŠQ•¨‰ñ”ğƒNƒ‰ƒX
*/
class CComObstacleAvoidance
{
public:

    void SetObstacles(const std::vector<SimpleObstacle>* obstacles)
    {
        m_pObstacles = obstacles;
    }
    void SetSelfRadius(float radius) { m_SelfRadius = radius; }
    void SetProbeDist(float dist) { m_ProbeDist = dist; }
    void SetProbeStep(float step) { m_ProbeStep = step; }
    void SetProbeAngle(float angle) { m_ProbeAngleRad = angle; }

    // ‘O•û‚ÉáŠQ•¨‚ª‚ ‚é‚©
    bool HasObstacleAhead(
        const D3DXVECTOR3& selfPos,
        float yaw,
        float probeDist,
        float step,
        float& outHitDist) const;

    // ŠëŒ¯ƒ][ƒ“”»’è
    bool IsInDangerZone(const D3DXVECTOR3& pos) const;

    // áŠQ•¨‚ğ”ğ‚¯‚é•ûŒü‚ğŒvZ
    float ComputeAvoidYaw(
        const D3DXVECTOR3& selfPos,
        float curYaw,
        float desiredYaw,
        float turnStep) const;

private:
    const std::vector<SimpleObstacle>* m_pObstacles;
    float m_SelfRadius;
    float m_ProbeDist;
    float m_ProbeStep;
    float m_ProbeAngleRad;
};