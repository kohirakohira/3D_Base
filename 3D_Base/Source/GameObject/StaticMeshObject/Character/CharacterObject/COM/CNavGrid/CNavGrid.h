#pragma once

//ライブラリ
#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>

//グリッド座標
struct GridPos
{
	int x, z;	//グリッド上の列のインデックス

	//同じマスかどうかの判定
	bool operator == (const GridPos& o) const { return x == o.x && z == o.z; }
	//同じマスではない
	bool operator !=(const GridPos& o) const { return !(*this == o); }
};

// GridPosをstd::unordered_mapのキーとして使うためのハッシュ関数オブジェクト
struct GridPosHash
{
	//x,z各座標のint からハッシュ値を作ってビットを混ぜて1つのハッシュ値にまとめる
	size_t operator() (const GridPos& p) const
	{
		return std::hash<int>()(p.x) ^ (std::hash<int>()(p.z) << 16);
	}
};

//経路探索結果
struct PathResult
{
	bool found = false;
	std::vector<D3DXVECTOR3> waypoints;	//ワールド座標のウェイポイント
};

class CNavGrid
{
public:
	CNavGrid();
	~CNavGrid() = default;

	//グリッド初期化
	void Initialize(float stageMinX, float stageMaxX,
		float stageMinZ, float stageMaxZ,
		float cellSize = 1.0f);

	//障害物を追加
	void AddObstacleAABB(const D3DXVECTOR3& minPos, const D3DXVECTOR3& maxPos, float margin = 0.0f);

	//障害物を追加.円
	void AddObstacleCircle(const D3DXVECTOR3& center, float radius);

	//障害物をクリア
	void ClearObstacles();

	//A*で経路探索
	PathResult FindPath(const D3DXVECTOR3& start, const D3DXVECTOR3& goal);

	//ワールド座標からグリッド座標
	GridPos WorldToGrid(const D3DXVECTOR3& gp) const;

	// グリッド座標からワールド座標
	D3DXVECTOR3 GridToWorld(const GridPos& gp) const;

	//セルが通行可能か
	bool IsWalkable(const GridPos& gp)const;
	bool IsWalkable(int x, int z) const;

	//デバッグ用
	void DebugPrint()const;

private:
	//A*のノード
	struct Node
	{
		GridPos pos;
		float g = 0;	//スタートからのコスト
		float f = 0;	
		GridPos parent;

		bool operator>(const Node& o) const { return f > o.f; }
	};

	//ヒューリスティック
	float Heuristic(const GridPos& a, const GridPos& b) const;

	//経路を再構築
	std::vector<GridPos> ReconstructPath(
		const std::unordered_map<GridPos, GridPos, GridPosHash>& cameFrom,
		GridPos current) const;

	//経路を滑らかにする
	std::vector<D3DXVECTOR3> SmoothPath(const std::vector<GridPos>& path)const;

	//2点間に障害物がないか
	bool HasLineOfSight(const GridPos& a, const GridPos& b) const;

private:
	std::vector<std::vector<bool>> m_Grid;

	float m_StageMinX, m_StageMaxX;
	float m_StageMinZ, m_StageMaxZ;
	float m_CellSize;
	int m_GridWidth;	//X方向のセル
	int m_GridHeight;	//Z方向のセル
};