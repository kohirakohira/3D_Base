#include "CNavGrid.h"
#include <algorithm>
#include <cstdio>

CNavGrid::CNavGrid()
    : m_StageMinX(-30.0f)
    , m_StageMaxX(30.0f)
    , m_StageMinZ(-30.0f)
    , m_StageMaxZ(30.0f)
    , m_CellSize(1.0f)
    , m_GridWidth(0), m_GridHeight(0)
{
}

void CNavGrid::Initialize(float stageMinX, float stageMaxX,
    float stageMinZ, float stageMaxZ,
    float cellSize)
{
    //ステージのワールド座標での範囲をメンバ変数に保存
    m_StageMinX = stageMinX;
    m_StageMaxX = stageMaxX;
    m_StageMinZ = stageMinZ;
    m_StageMaxZ = stageMaxZ;
    m_CellSize = cellSize;

    m_GridWidth = static_cast<int>((stageMaxX - stageMinX) / cellSize) + 1;
    m_GridHeight = static_cast<int>((stageMaxZ - stageMinZ) / cellSize) + 1;

    //全セルを通行可能で初期化
    m_Grid.assign(m_GridWidth, std::vector<bool>(m_GridHeight, true));

}

//障害物情報を全部リセットして、全マス通行可能に戻す
void CNavGrid::ClearObstacles()
{
    for (auto& row : m_Grid)
    {
        std::fill(row.begin(), row.end(), true);
    }
}

void CNavGrid::AddObstacleAABB(const D3DXVECTOR3& minPos, const D3DXVECTOR3& maxPos, float margin)
{
    //マージンを適用.実際より少し広めの設計
    GridPos gMin = WorldToGrid(D3DXVECTOR3(minPos.x - margin, 0, minPos.z - margin));
    GridPos gMax = WorldToGrid(D3DXVECTOR3(maxPos.x + margin, 0, maxPos.z + margin));

    //範囲内のセルをブロック
    for (int x = gMin.x; x <= gMax.x; ++x)
    {
        for (int z = gMin.z; z <= gMax.z; ++z)
        {
            //ステージ外にはみ出してもそのセルは無視
            if (x >= 0 && x < m_GridWidth && z >= 0 && z < m_GridHeight)
            {
                m_Grid[x] [z]= false;
            }
        }
    }
}

//円形の障害物をグリッド上のだいたい円形なブロック領域に変換する
void CNavGrid::AddObstacleCircle(const D3DXVECTOR3& center, float radius)
{
    //中心セルと何マス分の半径かを計算
    GridPos gCenter = WorldToGrid(center);
    int cellRadius = static_cast<int>(std::ceil(radius / m_CellSize));

    //候補のセルを中心からの相対オフセットで調べる
    for (int dx = -cellRadius; dx <= cellRadius; ++dx)
    {
        for (int dz = -cellRadius; dz <= cellRadius; ++dz)
        {
            int x = gCenter.x + dx;
            int z = gCenter.z + dz;

            //グリッドの範囲チェック
            if (x >= 0 && x < m_GridWidth && z >= 0 && z < m_GridHeight)
            {
                // セル中心との距離をチェック
                D3DXVECTOR3 cellCenter = GridToWorld({ x, z });
                float dist = std::sqrtf(
                    (cellCenter.x - center.x) * (cellCenter.x - center.x) +
                    (cellCenter.z - center.z) * (cellCenter.z - center.z));

                //セル中心が円の中ならそのセルをブロック
                if (dist <= radius)
                {
                    m_Grid[x][z] = false;
                }
            }
        }
    }
}

//ワールド座標posがGridPosに対応するかを計算して、
//はみ出しを防ぐためにグリッド範囲内にクランプする
GridPos CNavGrid::WorldToGrid(const D3DXVECTOR3& pos) const
{
    GridPos gp; //グリッド座標

    //ステージ左端からの距離をセルサイズで割って、何番目のセルかを求める
    gp.x = static_cast<int>((pos.x - m_StageMinX) / m_CellSize);
    gp.z = static_cast<int>((pos.z - m_StageMinZ) / m_CellSize);    //同じ

    //クランプ
    gp.x = std::max(0, std::min(gp.x, m_GridWidth - 1));
    gp.z = std::max(0, std::min(gp.z, m_GridHeight - 1));

    return gp;
}

//グリッドのマス番号からそのマスのワールド座標での中心位置を計算して返す
D3DXVECTOR3 CNavGrid::GridToWorld(const GridPos& gp) const
{
    return D3DXVECTOR3(
        m_StageMinX + (gp.x + 0.5f) * m_CellSize,   //0.5してセルの中心までずらす
        0.0f,
        m_StageMinZ + (gp.z + 0.5f) * m_CellSize
    );
}

//gridPos版のiswalkableからx,zに処理を丸投げ
bool CNavGrid::IsWalkable(const GridPos& gp) const
{
    return IsWalkable(gp.x, gp.z);
}

bool CNavGrid::IsWalkable(int x, int z) const
{
    if (x < 0 || x >= m_GridWidth || z < 0 || z >= m_GridHeight)
        return false;
    return m_Grid[x][z];
}

float CNavGrid::Heuristic(const GridPos& a, const GridPos& b) const
{
    float dx = static_cast<float>(a.x - b.x);
    float dz = static_cast<float>(a.z - b.z);
    return std::sqrtf(dx * dx + dz * dz);
}

PathResult CNavGrid::FindPath(const D3DXVECTOR3& start, const D3DXVECTOR3& goal)
{
    PathResult result;
    result.found = false;

    GridPos startGrid = WorldToGrid(start);
    GridPos goalGrid = WorldToGrid(goal);

    // スタートまたはゴールが通行不可の場合
    if (!IsWalkable(startGrid))
    {
        // 近くの通行可能セルを探す
        for (int r = 1; r <= 5; ++r)
        {
            for (int dx = -r; dx <= r; ++dx)
            {
                for (int dz = -r; dz <= r; ++dz)
                {
                    GridPos test = { startGrid.x + dx, startGrid.z + dz };
                    if (IsWalkable(test))
                    {
                        startGrid = test;
                        goto found_start;
                    }
                }
            }
        }
        return result;  // 通行可能なスタートが見つからない
    }
found_start:

    if (!IsWalkable(goalGrid))
    {
        // ゴール付近の通行可能セルを探す
        for (int r = 1; r <= 5; ++r)
        {
            for (int dx = -r; dx <= r; ++dx)
            {
                for (int dz = -r; dz <= r; ++dz)
                {
                    GridPos test = { goalGrid.x + dx, goalGrid.z + dz };
                    if (IsWalkable(test))
                    {
                        goalGrid = test;
                        goto found_goal;
                    }
                }
            }
        }
        return result;
    }
found_goal:

    // A* アルゴリズム
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::unordered_map<GridPos, float, GridPosHash> gScore;
    std::unordered_map<GridPos, GridPos, GridPosHash> cameFrom;

    Node startNode;
    startNode.pos = startGrid;
    startNode.g = 0;
    startNode.f = Heuristic(startGrid, goalGrid);
    openSet.push(startNode);
    gScore[startGrid] = 0;

    // 8方向の移動（斜め含む）
    const int DX[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
    const int DZ[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    const float COST[] = { 1.414f, 1.0f, 1.414f, 1.0f, 1.0f, 1.414f, 1.0f, 1.414f };

    while (!openSet.empty())
    {
        Node current = openSet.top();
        openSet.pop();

        if (current.pos == goalGrid)
        {
            // 経路が見つかった
            std::vector<GridPos> path = ReconstructPath(cameFrom, current.pos);
            result.waypoints = SmoothPath(path);
            result.found = true;
            return result;
        }

        // 既により良いパスがある場合はスキップ
        auto it = gScore.find(current.pos);
        if (it != gScore.end() && current.g > it->second)
            continue;

        // 隣接セルを探索
        for (int i = 0; i < 8; ++i)
        {
            GridPos neighbor = { current.pos.x + DX[i], current.pos.z + DZ[i] };

            if (!IsWalkable(neighbor))
                continue;

            // 斜め移動の場合、角を通り抜けないようにチェック
            if (i == 0 || i == 2 || i == 5 || i == 7)  // 斜め
            {
                if (!IsWalkable(current.pos.x + DX[i], current.pos.z) ||
                    !IsWalkable(current.pos.x, current.pos.z + DZ[i]))
                    continue;
            }

            float tentativeG = current.g + COST[i];

            auto git = gScore.find(neighbor);
            if (git == gScore.end() || tentativeG < git->second)
            {
                cameFrom[neighbor] = current.pos;
                gScore[neighbor] = tentativeG;

                Node neighborNode;
                neighborNode.pos = neighbor;
                neighborNode.g = tentativeG;
                neighborNode.f = tentativeG + Heuristic(neighbor, goalGrid);
                openSet.push(neighborNode);
            }
        }
    }

    return result;  // 経路が見つからなかった
}

std::vector<GridPos> CNavGrid::ReconstructPath(
    const std::unordered_map<GridPos, GridPos, GridPosHash>& cameFrom,
    GridPos current) const
{
    std::vector<GridPos> path;
    path.push_back(current);

    while (cameFrom.find(current) != cameFrom.end())
    {
        current = cameFrom.at(current);
        path.push_back(current);
    }

    std::reverse(path.begin(), path.end());
    return path;
}

bool CNavGrid::HasLineOfSight(const GridPos& a, const GridPos& b) const
{
    // Bresenhamのアルゴリズムで直線上のセルをチェック
    int x0 = a.x, z0 = a.z;
    int x1 = b.x, z1 = b.z;

    int dx = std::abs(x1 - x0);
    int dz = std::abs(z1 - z0);
    int sx = (x0 < x1) ? 1 : -1;
    int sz = (z0 < z1) ? 1 : -1;
    int err = dx - dz;

    while (true)
    {
        if (!IsWalkable(x0, z0))
            return false;

        if (x0 == x1 && z0 == z1)
            break;

        int e2 = 2 * err;
        if (e2 > -dz)
        {
            err -= dz;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            z0 += sz;
        }
    }

    return true;
}

std::vector<D3DXVECTOR3> CNavGrid::SmoothPath(const std::vector<GridPos>& path) const
{
    if (path.size() <= 2)
    {
        std::vector<D3DXVECTOR3> result;
        for (const auto& gp : path)
            result.push_back(GridToWorld(gp));
        return result;
    }

    // 視線が通る限りウェイポイントをスキップ
    std::vector<D3DXVECTOR3> smoothed;
    smoothed.push_back(GridToWorld(path[0]));

    size_t current = 0;
    while (current < path.size() - 1)
    {
        size_t farthest = current + 1;

        // 視線が通る最も遠い点を探す
        for (size_t i = current + 2; i < path.size(); ++i)
        {
            if (HasLineOfSight(path[current], path[i]))
            {
                farthest = i;
            }
        }

        smoothed.push_back(GridToWorld(path[farthest]));
        current = farthest;
    }

    return smoothed;
}

void CNavGrid::DebugPrint() const
{
    printf("NavGrid: %d x %d cells\n", m_GridWidth, m_GridHeight);
    for (int z = m_GridHeight - 1; z >= 0; --z)
    {
        for (int x = 0; x < m_GridWidth; ++x)
        {
            printf("%c", m_Grid[x][z] ? '.' : '#');
        }
        printf("\n");
    }
}