#include "GameObject/StaticMeshObject/Character/COM/NavGrid/NavGrid.h"

//1セルぶんの内部状態をもつ
struct Node
{
	float f;	//優先度キーに使う値
	float g;	//実コスト
	int px;		//親セルのグリッド座標
	int py;
	bool closed = false;	//クローズド集合フラグ
};

//aとbの距離をユークリッド距離でかえす
static inline float Heuristic(Vec2 a, Vec2 b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return std::hypot(dx, dy);
}


NavGrid::NavGrid(int w, int h, float cell)
    : m_H       ()
    , m_W       ()
    , m_Cell    ()
    , m_Block   ()

{
}

bool NavGrid::FindPath(Vec2 startCell, Vec2 goalCell, std::vector<Vec2>& outPath)
{
    //前回の計算結果を初期化する
    outPath.clear();
    if (IsBlocked(goalCell.x, goalCell.y)) return false;    //ゴールのセルが通行不可なら経路は存在しない

    const int totalCells = m_W * m_H;                           //グリッド全体のセル数を計算して使い回す
    constexpr float INF = std::numeric_limits<float>::infinity();   //到達していない

    //各セルの探索状態
    std::vector<Node> nodes(totalCells, 
        Node
        { 
            INF,    //未到達
            INF,
            -1,     //親なし
            -1,
            false   //まだ展開してない
        });

    //2Dから1Dインデックス
    auto indexOf = [&](int x, int y) { return y * m_W + x; };

    //オープンセット
    struct OpenItem { float f; int x; int y; };
    auto compareByF = [](const OpenItem& a, const OpenItem& b) { return a.f > b.f; };
    std::priority_queue<OpenItem, std::vector<OpenItem>, decltype(compareByF)> openSet(compareByF);

    // スタート初期化
    const int startIdx = indexOf(startCell.x, startCell.y);
    nodes[startIdx].g = 0.f;
    nodes[startIdx].f = Heuristic(startCell, goalCell);
    openSet.push({ nodes[startIdx].f, startCell.x, startCell.y });

    //近傍とコスト
    constexpr int kNeighbor8[8][2] = {
        {+1, 0}, {-1, 0}, {0,+1}, {0,-1},
        {+1,+1}, {+1,-1}, {-1,+1}, {-1,-1}
    };
    constexpr float COST_STRAIGHT = 1.0f;
    constexpr float COST_DIAGONAL = 1.41421356f; //√2

    while (!openSet.empty())
    {
        const OpenItem curItem = openSet.top(); openSet.pop();
        const int cx = curItem.x;
        const int cy = curItem.y;

        Node& curNode = nodes[indexOf(cx, cy)];
        if (curNode.closed) continue;
        curNode.closed = true;

        //ゴール到達経路復元
        if (cx == goalCell.x && cy == goalCell.y)
        {
            int rx = cx, ry = cy;
            while (!(rx == startCell.x && ry == startCell.y)) {
                outPath.push_back({ rx, ry });
                const Node& n = nodes[indexOf(rx, ry)];
                rx = n.px; ry = n.py;
            }
            outPath.push_back(startCell);
            std::reverse(outPath.begin(), outPath.end());
            return true;
        }

        // 近傍展開
        for (const auto& off : kNeighbor8)
        {
            const int nx = cx + off[0];
            const int ny = cy + off[1];
            if (IsBlocked(nx, ny)) continue;

            const bool isStraight = (off[0] == 0 || off[1] == 0);
            const float stepCost = isStraight ? COST_STRAIGHT : COST_DIAGONAL;

            const float tentativeG = curNode.g + stepCost;
            Node& nb = nodes[indexOf(nx, ny)];

            if (tentativeG < nb.g)
            {
                nb.g = tentativeG;
                nb.f = tentativeG + Heuristic({ nx, ny }, goalCell);
                nb.px = cx;
                nb.py = cy;
                openSet.push({ nb.f, nx, ny });
            }
        }
    }
    return false;
}
