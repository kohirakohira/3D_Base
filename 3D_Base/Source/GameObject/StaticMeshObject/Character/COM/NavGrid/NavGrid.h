#pragma once

#include <vector>
#include <memory>
#include <queue>
#include <limits>
#include <cmath>

//ナビグリッドクラス.経路探索

//ナビ用グリッドのセル座標
struct Vec2
{
	int x;
	int y;
};

class NavGrid
{
public:
	//w列.h行.cell1セルのワールド長さ
	NavGrid(int w, int h, float cell);

	//読み取りアクセサ
	int W() const { return m_W; }
	int H() const { return m_H; }
	float Cell() const { return m_Cell; }

	//ワールド座標をグリッドのセル座標に変換する
	//floorはx以下で最大の整数へ切り下げる
	Vec2 ToCell(float wx, float wz) const {
		return { int(std::floor(wx / m_Cell)),int(std::floor(wz / m_Cell)) };
	}

	//セル座標からそのセルのワールド中心座標を計算して返す
	void ToWorldCenter(const Vec2& c, float& wx, float& wz) const
	{
		wx = (c.x + 0.5f) * m_Cell;
		wz = (c.y + 0.5f) * m_Cell;
	}

	//障害物マーキング.このセルを回避する
	void MarkBlocked(int x, int y)
	{
		if (0 <= x && x < m_W && 0 <= y && y < m_H)
		{
			m_Block[y * m_W + x] = true;	//2次元を一次元配列に変換して、通行不可に設定
		}
	}

	//セルが通行不可かどうかを返す関数
	bool IsBlocked(int x, int y) const
	{
		return !(0 <= x && x < m_W && 0 <= y && y < m_H) || m_Block[y * m_W + x];
	}

	//A*.boolはパスが見つかったかどうか
	bool FindPath(Vec2 start, Vec2 goal, std::vector<Vec2>& out);


private:
	int		m_W;					//グリッドの横セル数.幅
	int		m_H;					//グリッドの縦セル数.高さ
	float	m_Cell;					//1セルのワールド長さ
	std::vector<uint8_t> m_Block;	//占有情報を平坦化して保持するバッファ

};