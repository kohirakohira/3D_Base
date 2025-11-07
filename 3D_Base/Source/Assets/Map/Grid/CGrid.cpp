#include "CGrid.h"

CGrid::CGrid(float width, float height, float cellSize)
	: m_GridCell			()
{
	//初期化.
	m_GridCell = { width, height, cellSize };
	//グリッドを実際に作る※内部は2次元配列(例：m_Cells[z][x]).
	m_Cells.resize(height, std::vector<Cell>(width));

}

CGrid::~CGrid()
{

}

//座標からグリッドインデックスに変換.
std::pair<float, float> CGrid::WorldToGrid(float x, float z) const
{
	//3D座標→マップ上のマスに変換.
	float gx = x / m_GridCell.CellSize;
	float gz = z / m_GridCell.CellSize;
	return {gx, gz};
}

//グリッドからワールド座標に変換.
D3DXVECTOR3 CGrid::GridToWorld(float gx, float gz) const
{
	//グリッド座標をワールド座標に戻す関数※「 + m_GridCell.m_CellSize / 2 」：マスの中心に置くため.
	D3DXVECTOR3 world = {
		gx * m_GridCell.CellSize + m_GridCell.CellSize / 2.0f,
		0.0f,
		gz * m_GridCell.CellSize + m_GridCell.CellSize / 2.0f
	};
	return world;
}

//通れるか判定関数.
bool CGrid::IsImpassable(float gx, float gz)
{
	//マップ外？障害物？を判定.
	if (gx < 0.0f || gz < 0.0f || gx >= m_GridCell.Width || gz >= m_GridCell.Height)
	{
		//マップ外・障害物.
		return true;
	}
	//上記以外.
	return m_Cells[gz][gx].Blocked;
}

//障害物を設定.
void CGrid::SetBlocked(float gx, float gz, bool blocked)
{
	if ()
	{

	}
}
