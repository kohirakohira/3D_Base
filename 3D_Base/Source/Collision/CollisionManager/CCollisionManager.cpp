#include "CCollisionManager.h"

CCollisionManager::CCollisionManager()
	: m_pStaticBlast		()

	// 壁
	, m_pWallTop			()
	, m_pWallBottom			()
	, m_pWallLeft			()
	, m_pWallRight			()

	// 地面
	, m_pGround				()

	// 木箱
	, m_pWoodBoxTopLeft		()
	, m_pWoodBoxTopRight	()
	, m_pWoodBoxCenter		()
	, m_pWoodBoxBottomLeft	()
	, m_pWoodBoxBottomRight	()

	// 弾マネージャー
	, m_pShotManager		()

	// プレイヤーマネージャー
	, m_pPlayerManager		()

	// 爆風当たり判定マネージャー
	, m_pBlastManager		()

	// アイテムボックスマネージャー
	, m_pItemBoxManager		()
{
}

CCollisionManager::~CCollisionManager()
{
}

void CCollisionManager::Update()
{
	// 壁とプレイヤーの当たり判定
	WalltoPlayer();

	// 壁と弾の当たり判定
	WalltoShot();

	// プレイヤーとプレイヤー当たり判定判別
	PlayertoPlayer();

	// プレイヤーとアイテムボックス
	PlayertoItemBox();

	// プレイヤーと弾
	PlayertoShot();

	// 弾と弾
	//ShottoShot(); // 激重コード(要改善)

	// 箱とプレイヤー
	WoodBoxtoPlayer();

	// 木箱と弾
	WoodBoxtoShot();

	// 地面と弾
	GroundtoShot();

	// 地面とアイテムボックス
	GroundtoItemBox();

	//爆風とプレイヤーの当たり判定.
	PlayertoBlast();
}

HRESULT CCollisionManager::LoadData()
{
//--------------------------------------------------------------------------.
// 	   メッシュの読み込み
//--------------------------------------------------------------------------.
	// 爆風
	m_pStaticBlast->Init(_T("Data\\Mesh\\Static\\Bullet\\Red\\Ball.x"));

	return S_OK;
}

void CCollisionManager::WalltoPlayer()
{
	// 押し返しの強さ
	const float pushStrength = m_pPlayerManager->GetTuning().moveSpeed;

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// i 番のプレイヤーを取得
		auto player = m_pPlayerManager->GetControlPlayer(i);
		auto Coll = player->GetBody()->GetCollider();

		// 押し返すための変数
		D3DXVECTOR3 push(0.0f, 0.0f, 0.0f);

		// 車体が壁と接触したとき
		if (Coll->CheckCollision(*m_pWallTop->GetCollider()))
		{
			push.z -= pushStrength;
		}
		if (Coll->CheckCollision(*m_pWallBottom->GetCollider()))
		{
			push.z += pushStrength;
		}
		if (Coll->CheckCollision(*m_pWallLeft->GetCollider()))
		{
			push.x += pushStrength;
		}
		if (Coll->CheckCollision(*m_pWallRight->GetCollider()))
		{
			push.x -= pushStrength;
		}

		// 押し返しを正規化
		if (D3DXVec3Length(&push) > 0.f)
		{
			D3DXVec3Normalize(&push, &push);
			push *= pushStrength;
		}

		// 壁に当たった時に押し返す
		player->GetBody()->PushBack(push);
		player->GetCannon()->PushBack(push);
	}
}

// 壁と弾の当たり判定
void CCollisionManager::WalltoShot()
{
	for (int i = 0; i < ShotMax; i++)
	{
		// ショットのコライダー取得
		auto Shots = m_pShotManager->GetShot();
		auto ShotsColl = Shots[i]->GetCollider();

		// 壁が弾と接触したとき
		if (ShotsColl->CheckCollision(*m_pWallTop->GetCollider()))
		{
			//動的に作成.
			m_pBlastManager->Create(
				Shots[i]->GetPosition(),
				true,
				m_pStaticBlast);

			Shots[i]->HitShot();
		}
		if (ShotsColl->CheckCollision(*m_pWallBottom->GetCollider()))
		{
			//動的に作成.
			m_pBlastManager->Create(
				Shots[i]->GetPosition(),
				true,
				m_pStaticBlast);

			Shots[i]->HitShot();
		}
		if (ShotsColl->CheckCollision(*m_pWallLeft->GetCollider()))
		{
			//動的に作成.
			m_pBlastManager->Create(
				Shots[i]->GetPosition(),
				true,
				m_pStaticBlast);

			Shots[i]->HitShot();
		}
		if (ShotsColl->CheckCollision(*m_pWallRight->GetCollider()))
		{
			//動的に作成.
			m_pBlastManager->Create(
				Shots[i]->GetPosition(),
				true,
				m_pStaticBlast);

			Shots[i]->HitShot();
		}
	}
}

// プレイヤーとプレイヤー当たり判別
void CCollisionManager::PlayertoPlayer()
{
	const float pushStrength = m_pPlayerManager->GetTuning().moveSpeed;

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// プレイヤーAのコライダー取得
		auto playerA = m_pPlayerManager->GetControlPlayer(i);
		auto CollA = playerA->GetBody()->GetCollider();

		for (int j = 0; j < PLAYER_MAX; j++)
		{
			// 自分自身との判定をスキップ
			if (i == j) continue;

			// プレイヤーBのコライダー取得
			auto playerB = m_pPlayerManager->GetControlPlayer(j);
			auto CollB = playerB->GetBody()->GetCollider();

			if (CollA->CheckCollision(*CollB))
			{
				// 衝突時の押し返し処理例
				D3DXVECTOR3 push = playerA->GetBody()->GetPosition() - playerB->GetBody()->GetPosition();

				// pushベクトルを正規化して押し返しの強さをかける
				float length = D3DXVec3Length(&push);
				if (length > 0.0001f)
				{
					push /= length;
					push *= pushStrength;
					playerA->GetBody()->PushBack(push);
				}
			}
		}
	}
}

// プレイヤーとアイテムボックス
void CCollisionManager::PlayertoItemBox()
{
	for (int PlayerIndex = 0; PlayerIndex < PLAYER_MAX; ++PlayerIndex)
	{
		// i 番のプレイヤーを取得
		auto player = m_pPlayerManager->GetControlPlayer(PlayerIndex);
		auto Coll = player->GetBody()->GetCollider();

		for (size_t ItemIndex = 0; ItemIndex < m_pItemBoxManager->GetItem().size(); ItemIndex++)
		{
			// プレイヤーがアイテムと接触したとき
			if (Coll->CheckCollision(*m_pItemBoxManager->GetItem()[ItemIndex]->GetCollider()))
			{
				//アイテムの中を設定してあげる.
				m_pItemBoxManager->SetItemInfo(ItemIndex);
				//画面から消す.
				m_pItemBoxManager->GetItem()[ItemIndex]->HitPlayer();

				//無敵処理.
				//プレイヤーに設定.
				if (m_pItemBoxManager->GetItemInfo(ItemIndex).m_ShieldFlag != false)
				{

				}

				//速度設定.
				//プレイヤーに設定.
				if (m_pItemBoxManager->GetItemInfo(ItemIndex).m_Speed > 0.0f)
				{
					//プレイヤーの速度を設定.
					const TankTuning Info = { m_pItemBoxManager->GetItemInfo(ItemIndex).m_Speed, 0.03f, 0.03f, 0.3f };
					//プレイヤーの情報を設定.
					m_pPlayerManager->SetPlayerTuning(PlayerIndex, Info);
				}

				//攻撃力設定.
				//弾に設定.
				if (m_pItemBoxManager->GetItemInfo(ItemIndex).m_Power > 0.0f)
				{

				}

				//爆風の半径設定.
				//爆風に設定.
				if (m_pItemBoxManager->GetItemInfo(ItemIndex).m_Blast > 0.0f)
				{

				}

				//装填時短設定.
				//弾に設定.
				if (m_pItemBoxManager->GetItemInfo(ItemIndex).m_Reload > 0.0f)
				{

				}
			}
		}
	}
}

// プレイヤーと弾
void CCollisionManager::PlayertoShot()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// プレイヤーのコライダー取得
		auto player = m_pPlayerManager->GetControlPlayer(i);
		auto Coll = player->GetBody()->GetCollider();

		for (int i = 0; i < ShotMax; i++)
		{
			// ショットのコライダー取得
			auto Shots = m_pShotManager->GetShot();
			auto ShotsColl = Shots[i]->GetCollider();

			if (ShotsColl->CheckCollision(*Coll))
			{
				//動的に作成.
				m_pBlastManager->Create(
					Shots[i]->GetPosition(),
					true,
					m_pStaticBlast);

				Shots[i]->HitShot();
			}
		}
	}
}

// 弾と弾
void CCollisionManager::ShottoShot()
{
	for (int i = 0; i < ShotMax; i++)
	{
		// ショットAのコライダー取得
		auto ShotsA = m_pShotManager->GetShot();
		auto ShotsCollA = ShotsA[i]->GetCollider();

		for (int j = 0; j < ShotMax; j++)
		{
			// 自分自身との判定をスキップ
			if (i == j) continue;

			// ショットBのコライダー取得
			auto ShotsB = m_pShotManager->GetShot();
			auto ShotsCollB = ShotsB[i]->GetCollider();

			if (ShotsCollA->CheckCollision(*ShotsCollB))
			{
				ShotsA[i]->HitShot();
				ShotsB[i]->HitShot();
			}
		}
	}
}

// 木箱とプレイヤー
void CCollisionManager::WoodBoxtoPlayer()
{
	// 押し返しの強さ
	const float pushStrength = m_pPlayerManager->GetTuning().moveSpeed;

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// i 番のプレイヤーを取得
		auto player = m_pPlayerManager->GetControlPlayer(i);
		auto Coll = player->GetBody()->GetCollider();

		// 押し返すための変数
		D3DXVECTOR3 push(0.0f, 0.0f, 0.0f);

		// 車体が壁と接触したとき
		// 左上
		if (Coll->CheckCollision(*m_pWoodBoxTopLeft->GetCollider()))
		{
			// 衝突時の押し返し処理例
			D3DXVECTOR3 push = player->GetBody()->GetPosition() - m_pWoodBoxTopLeft->GetPosition();

			// pushベクトルを正規化して押し返しの強さをかける
			float length = D3DXVec3Length(&push);
			if (length > 0.0001f)
			{
				push /= length;
				push *= pushStrength;
				player->GetBody()->PushBack(push);
			}
		}
		// 右上
		if (Coll->CheckCollision(*m_pWoodBoxTopRight->GetCollider()))
		{
			// 衝突時の押し返し処理例
			D3DXVECTOR3 push = player->GetBody()->GetPosition() - m_pWoodBoxTopRight->GetPosition();

			// pushベクトルを正規化して押し返しの強さをかける
			float length = D3DXVec3Length(&push);
			if (length > 0.0001f)
			{
				push /= length;
				push *= pushStrength;
				player->GetBody()->PushBack(push);
			}
		}
		// 中央
		if (Coll->CheckCollision(*m_pWoodBoxCenter->GetCollider()))
		{
			// 衝突時の押し返し処理例
			D3DXVECTOR3 push = player->GetBody()->GetPosition() - m_pWoodBoxCenter->GetPosition();

			// pushベクトルを正規化して押し返しの強さをかける
			float length = D3DXVec3Length(&push);
			if (length > 0.0001f)
			{
				push /= length;
				push *= pushStrength;
				player->GetBody()->PushBack(push);
			}
		}
		// 左下
		if (Coll->CheckCollision(*m_pWoodBoxBottomLeft->GetCollider()))
		{
			// 衝突時の押し返し処理例
			D3DXVECTOR3 push = player->GetBody()->GetPosition() - m_pWoodBoxBottomLeft->GetPosition();

			// pushベクトルを正規化して押し返しの強さをかける
			float length = D3DXVec3Length(&push);
			if (length > 0.0001f)
			{
				push /= length;
				push *= pushStrength;
				player->GetBody()->PushBack(push);
			}
		}
		// 右下
		if (Coll->CheckCollision(*m_pWoodBoxBottomRight->GetCollider()))
		{
			// 衝突時の押し返し処理例
			D3DXVECTOR3 push = player->GetBody()->GetPosition() - m_pWoodBoxBottomRight->GetPosition();

			// pushベクトルを正規化して押し返しの強さをかける
			float length = D3DXVec3Length(&push);
			if (length > 0.0001f)
			{
				push /= length;
				push *= pushStrength;
				player->GetBody()->PushBack(push);
			}
		}

		// 押し返しを正規化
		if (D3DXVec3Length(&push) > 0.f)
		{
			D3DXVec3Normalize(&push, &push);
			push *= pushStrength;
		}

		// 壁に当たった時に押し返す
		player->GetBody()->PushBack(push);
		player->GetCannon()->PushBack(push);
	}
}

// 木箱と弾
void CCollisionManager::WoodBoxtoShot()
{
	for (int i = 0; i < ShotMax; i++)
	{
		// ショットのコライダー取得
		auto Shots = m_pShotManager->GetShot();
		auto ShotsColl = Shots[i]->GetCollider();

		// 壁が弾と接触したとき
		if (ShotsColl->CheckCollision(*m_pWoodBoxTopLeft->GetCollider()))
		{
			// 動的に作成
			m_pBlastManager->Create(
				Shots[i]->GetPosition(),
				true,
				m_pStaticBlast);

			Shots[i]->HitShot();
		}
		if (ShotsColl->CheckCollision(*m_pWoodBoxTopRight->GetCollider()))
		{
			//動的に作成.
			m_pBlastManager->Create(
				Shots[i]->GetPosition(),
				true,
				m_pStaticBlast);

			Shots[i]->HitShot();
		}
		if (ShotsColl->CheckCollision(*m_pWoodBoxCenter->GetCollider()))
		{
			//動的に作成.
			m_pBlastManager->Create(
				Shots[i]->GetPosition(),
				true,
				m_pStaticBlast);

			Shots[i]->HitShot();
		}
		if (ShotsColl->CheckCollision(*m_pWoodBoxBottomLeft->GetCollider()))
		{
			//動的に作成.
			m_pBlastManager->Create(
				Shots[i]->GetPosition(),
				true,
				m_pStaticBlast);

			Shots[i]->HitShot();
		}
		if (ShotsColl->CheckCollision(*m_pWoodBoxBottomRight->GetCollider()))
		{
			//動的に作成.
			m_pBlastManager->Create(
				Shots[i]->GetPosition(),
				true,
				m_pStaticBlast);

			Shots[i]->HitShot();
		}
	}
}

// 地面と弾
void CCollisionManager::GroundtoShot()
{
	for (int i = 0; i < ShotMax; i++)
	{
		// ショットのコライダー取得
		auto Shots = m_pShotManager->GetShot();
		auto ShotsColl = Shots[i]->GetCollider();

		if (ShotsColl->CheckCollision(*m_pGround->GetCollider()))
		{
			// 動的に作成
			m_pBlastManager->Create(
				Shots[i]->GetPosition(),
				true,
				m_pStaticBlast);
			Shots[i]->HitShot();
		}
	}
}

// 地面とアイテムボックス
void CCollisionManager::GroundtoItemBox()
{
	for (auto& item : m_pItemBoxManager->GetItem())
	{
		if (item->GetCollider()->CheckCollision(*m_pGround->GetCollider()))
		{
			// アイテムボックスの処理を入れる
			item->SetGravity(true);
		}
	}
}

// 爆風とプレイヤーの当たり判定
void CCollisionManager::PlayertoBlast()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		//i番目のプレイヤーを取得.
		auto player = m_pPlayerManager->GetControlPlayer(i);
		auto Coll = player->GetBody()->GetCollider();

		if (m_pBlastManager->GetBlastFlag() == true)
		{
			////車体が爆風と接触したとき.
			if (Coll->CheckCollision(*m_pBlastManager->GetCollider()))
			{
				m_pBlastManager->HitBlast(i);
			}
		}
	}
}
