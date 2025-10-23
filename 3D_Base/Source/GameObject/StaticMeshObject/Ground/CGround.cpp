#include "CGround.h"

CGround::CGround()
	: m_pPlayerManager	()
{
}

CGround::~CGround()
{
	m_pPlayerManager = nullptr;
}

void CGround::Update()
{
	//地面をスクロールさせる
	m_vPosition.z -= 0.2f;
	if (m_vPosition.z < -100.f) {
		m_vPosition.z = 0.f;
	}

}

void CGround::RespawnArea()
{
	struct Area {
		bool taken = false;
		D3DXVECTOR3 RespawnPos; // リスポーン位置
	};

	// エリア4つを定義（マップの座標系に合わせて調整）
	Area areas[4];
	areas[0].RespawnPos = { -30.f, 0.f,  30.f }; // 左上
	areas[1].RespawnPos = { 30.f, 0.f,  30.f }; // 右上
	areas[2].RespawnPos = { -30.f, 0.f, -30.f }; // 左下
	areas[3].RespawnPos = { 30.f, 0.f, -30.f }; // 右下

	// 各プレイヤーがどのエリアにいるか調べる
	for (int i = 0; i < 4; ++i)
	{
		// i 番のプレイヤーを取得
		auto player = m_pPlayerManager->GetControlPlayer(i);
		auto PPos = player->GetBody()->GetPosition();

		int areaIndex = GetAreaIndex(PPos.x, PPos.z);

		areas[areaIndex].taken = true;
	}

	// 空いているエリアを探す
	int freeIndex = -1;
	for (int i = 0; i < 4; ++i)
	{
		if (!areas[i].taken)
		{
			freeIndex = i;
			break;
		}
	}

	if (freeIndex == -1)
	{
		// 全て埋まっている場合 → ランダムなどで選ぶ
		freeIndex = rand() % 4;
	}

	// 各プレイヤーがどのエリアにいるか調べる
	for (int i = 0; i < 4; ++i)
	{
		// リスポーンしたいプレイヤーを取得（例：0番）
		auto respawnPlayer = m_pPlayerManager->GetControlPlayer(i);
		if (respawnPlayer)
		{
			respawnPlayer->SetPosition(areas[freeIndex].RespawnPos);
		}
	}
}

int CGround::GetAreaIndex(float x, float z)
{
	// 四捨五入の座標を使用
	float rx = std::round(x);
	float rz = std::round(z);

	// もし四捨五入結果が0なら、適当に片方に寄せる
	if (rx == 0) rx = (x >= 0) ? 1 : -1;
	if (rz == 0) rz = (z >= 0) ? 1 : -1;

	// これで確実に x,z は ±1 のどちらかに分類できる
	if (rx < 0 && rz > 0) return 0; // 左上
	if (rx > 0 && rz > 0) return 1; // 右上
	if (rx < 0 && rz < 0) return 2; // 左下
	if (rx > 0 && rz < 0) return 3; // 右下
}
