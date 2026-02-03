#include "CSimplePathfinder.h"
#include <queue>
#include <cmath>
#include <algorithm>

CSimplePathfinder::CSimplePathfinder()
    : m_GridSize(10)
    , m_WorldSize(40.0f)
    , m_CellSize(4.0f)
{
}

void CSimplePathfinder::Initialize(int gridSize, float worldSize)
{
    m_GridSize = gridSize;
    m_WorldSize = worldSize;
    m_CellSize = worldSize / gridSize;  //1マスがワールド上でなんユニットかを計算

    //基本は全部通行可能状態にしておく
    m_Grid.resize(gridSize, std::vector<bool>(gridSize, true));
}

void CSimplePathfinder::WorldToGrid(const D3DXVECTOR3& world, int& outX, int& outY) const
{
    float halfWorld = m_WorldSize * 0.5f;

    outX = static_cast<int>((world.x + halfWorld) / m_CellSize);
    outY = static_cast<int>((world.z + halfWorld) / m_CellSize);

    outX = std::max(0, std::min(outX, m_GridSize - 1));
    outY = std::max(0, std::min(outY, m_GridSize - 1));
}

D3DXVECTOR3 CSimplePathfinder::GridToWorld(int x, int y) const
{
    float halfWorld = m_WorldSize * 0.5f;
    float halfCell = m_CellSize * 0.5f;

    return D3DXVECTOR3(
        x * m_CellSize - halfWorld + halfCell,
        0.0f,
        y * m_CellSize - halfWorld + halfCell
    );
}

// 障害物1つをグリッドに反映
void CSimplePathfinder::MarkObstacle(const D3DXVECTOR3& pos, float radius)
{
    int cx, cy;
    WorldToGrid(pos, cx, cy);

    int radiusCells = static_cast<int>(std::ceil(radius / m_CellSize)) + 1;

    for (int dy = -radiusCells; dy <= radiusCells; ++dy)
    {
        for (int dx = -radiusCells; dx <= radiusCells; ++dx)
        {
            int nx = cx + dx;
            int ny = cy + dy;

            if (nx < 0 || nx >= m_GridSize || ny < 0 || ny >= m_GridSize)
                continue;

            D3DXVECTOR3 cellCenter = GridToWorld(nx, ny);
            float distX = cellCenter.x - pos.x;
            float distZ = cellCenter.z - pos.z;
            float dist = std::sqrtf(distX * distX + distZ * distZ);

            if (dist < radius + m_CellSize * 0.5f)
            {
                m_Grid[nx][ny] = false;
            }
        }
    }
}

bool CSimplePathfinder::IsWalkable(int x, int y) const
{
    if (x < 0 || x >= m_GridSize || y < 0 || y >= m_GridSize)
        return false;
    return m_Grid[x][y];
}

float CSimplePathfinder::Heuristic(int x1, int y1, int x2, int y2) const
{
    return static_cast<float>(std::abs(x2 - x1) + std::abs(y2 - y1));
}

bool CSimplePathfinder::FindPath(
    const D3DXVECTOR3& start,
    const D3DXVECTOR3& goal,
    std::deque<D3DXVECTOR3>& outPath)
{
    outPath.clear();

    int sx, sy, gx, gy;
    WorldToGrid(start, sx, sy);
    WorldToGrid(goal, gx, gy);

    if (!IsWalkable(sx, sy) || !IsWalkable(gx, gy))
    {
        outPath.push_back(goal);
        return false;
    }

    if (sx == gx && sy == gy)
    {
        outPath.push_back(goal);
        return true;
    }

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;
    std::vector<std::vector<bool>> closed(m_GridSize, std::vector<bool>(m_GridSize, false));
    std::vector<std::vector<std::pair<int, int>>> parent(
        m_GridSize, std::vector<std::pair<int, int>>(m_GridSize, { -1, -1 }));

    Node startNode;
    startNode.x = sx;
    startNode.y = sy;
    startNode.g = 0.0f;
    startNode.h = Heuristic(sx, sy, gx, gy);
    startNode.f = startNode.g + startNode.h;
    openList.push(startNode);

    const int dx[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
    const int dy[] = { 1, 1, 0, -1, -1, -1, 0, 1 };
    const float cost[] = { 1.0f, 1.414f, 1.0f, 1.414f, 1.0f, 1.414f, 1.0f, 1.414f };

    bool found = false;

    while (!openList.empty())
    {
        Node current = openList.top();
        openList.pop();

        int cx = current.x;
        int cy = current.y;

        if (closed[cx][cy]) continue;
        closed[cx][cy] = true;

        if (cx == gx && cy == gy)
        {
            found = true;
            break;
        }

        for (int i = 0; i < 8; ++i)
        {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            if (!IsWalkable(nx, ny)) continue;
            if (closed[nx][ny]) continue;

            if (i % 2 == 1)
            {
                if (!IsWalkable(cx + dx[i], cy) || !IsWalkable(cx, cy + dy[i]))
                    continue;
            }

            float newG = current.g + cost[i];

            Node neighbor;
            neighbor.x = nx;
            neighbor.y = ny;
            neighbor.g = newG;
            neighbor.h = Heuristic(nx, ny, gx, gy);
            neighbor.f = neighbor.g + neighbor.h;

            if (parent[nx][ny].first == -1 || newG < current.g)
            {
                parent[nx][ny] = { cx, cy };
                openList.push(neighbor);
            }
        }
    }

    if (!found)
    {
        outPath.push_back(goal);
        return false;
    }

    std::deque<D3DXVECTOR3> reversePath;
    int px = gx, py = gy;
    while (px != -1 && py != -1)
    {
        reversePath.push_front(GridToWorld(px, py));
        auto p = parent[px][py];
        if (p.first == sx && p.second == sy)
        {
            break;
        }
        px = p.first;
        py = p.second;
    }

    if (!reversePath.empty())
    {
        reversePath.back() = goal;
    }

    outPath = reversePath;
    SmoothPath(outPath);

    return true;
}

void CSimplePathfinder::SmoothPath(std::deque<D3DXVECTOR3>& path) const
{
    if (path.size() <= 2) return;

    std::deque<D3DXVECTOR3> smoothed;
    smoothed.push_back(path.front());

    size_t current = 0;
    while (current < path.size() - 1)
    {
        size_t farthest = current + 1;
        for (size_t i = current + 2; i < path.size(); ++i)
        {
            bool canGo = true;
            D3DXVECTOR3 from = path[current];
            D3DXVECTOR3 to = path[i];

            float dist = std::sqrtf(
                (to.x - from.x) * (to.x - from.x) +
                (to.z - from.z) * (to.z - from.z));

            int steps = static_cast<int>(dist / (m_CellSize * 0.5f));
            for (int s = 1; s <= steps && canGo; ++s)
            {
                float t = static_cast<float>(s) / steps;
                D3DXVECTOR3 p;
                p.x = from.x + (to.x - from.x) * t;
                p.y = 0.0f;
                p.z = from.z + (to.z - from.z) * t;

                int gx, gy;
                WorldToGrid(p, gx, gy);
                if (!IsWalkable(gx, gy))
                {
                    canGo = false;
                }
            }

            if (canGo)
            {
                farthest = i;
            }
        }

        current = farthest;
        smoothed.push_back(path[current]);
    }

    path = smoothed;
}