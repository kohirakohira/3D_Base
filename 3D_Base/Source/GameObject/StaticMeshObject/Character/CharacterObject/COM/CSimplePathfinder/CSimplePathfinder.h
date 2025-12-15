#pragma once
#include <d3dx9math.h>
#include <vector>
#include <deque>

//前方宣言ではなく、CComPlayerの型を使う
//SimpleObstacle を独自定義せず、外部から受け取る形にする

// 軽量グリッドA*経路探索
class CSimplePathfinder
{
public:
    // 共通で使う障害物構造体
    struct SimpleObstacle
    {
        D3DXVECTOR3 pos;
        float radius;
    };

    CSimplePathfinder();
    ~CSimplePathfinder() = default;

    void Initialize(int gridSize, float worldSize);

    //テンプレートで任意のSimpleObstacle型を受け入れる
    template<typename ObstacleType>
    void UpdateObstacles(const std::vector<ObstacleType>* obstacles);

    bool FindPath(const D3DXVECTOR3& start,
        const D3DXVECTOR3& goal,
        std::deque<D3DXVECTOR3>& outPath);

    bool IsWalkable(int x, int y) const;
    void WorldToGrid(const D3DXVECTOR3& world, int& outX, int& outY) const;
    D3DXVECTOR3 GridToWorld(int x, int y) const;

private:
    // 内部用障害物1つでグリッドを更新
    void MarkObstacle(const D3DXVECTOR3& pos, float radius);

    struct Node
    {
        int x, y;
        float g, h, f;
        int parentX, parentY;
        bool operator>(const Node& other) const { return f > other.f; }
    };

    float Heuristic(int x1, int y1, int x2, int y2) const;
    void SmoothPath(std::deque<D3DXVECTOR3>& path) const;

    int m_GridSize;
    float m_WorldSize;
    float m_CellSize;
    std::vector<std::vector<bool>> m_Grid;
};

//テンプレートにしてManagerでも使えるように
template<typename ObstacleType>
void CSimplePathfinder::UpdateObstacles(const std::vector<ObstacleType>* obstacles)
{
    // 全てリセット
    for (auto& row : m_Grid)
    {
        std::fill(row.begin(), row.end(), true);
    }

    if (!obstacles) return;

    // 各障害物を処理
    for (const auto& ob : *obstacles)
    {
        MarkObstacle(ob.pos, ob.radius);
    }
}