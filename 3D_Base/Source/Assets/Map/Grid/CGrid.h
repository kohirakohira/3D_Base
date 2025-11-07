#pragma once
//STL.
#include <iostream>
#include <vector>
#include <utility>

//=====================================================================
// グリッドクラス※地面を分割して判定に使用.
//=====================================================================
class CGrid
{
public:
	//グリッドの1マス.
	struct Cell
	{
		bool Blocked;	//障害物があるか.
		bool Item;		//アイテムがあるか.

		//初期化.
		Cell()
			: Blocked	(false)
			, Item		(false)
		{}
	};
public:
	CGrid(float width, float height, float cellSize);
	~CGrid();

	//座標からグリッドインデックスに変換.
	std::pair<float, float> WorldToGrid(float x, float z) const;

	//グリッドからワールド座標に変換.
	D3DXVECTOR3 GridToWorld(float gx, float gz) const;

	//通れるか判定関数.
	bool IsImpassable(float gx, float gz);

	//障害物を設定.
	void SetBlocked(float gx, float gz, bool blocked);

private:
	//幅・高さ.
	struct Argument
	{
		float Width;
		float Height;
		float CellSize;
	};
	//セル情報.
	Argument m_GridCell;

	//セルの数※縦×横.
	std::vector<std::vector<Cell>> m_Cells;

};