#include "CCollisionManager.h"
//-----サウンド-----
#include "Assets//Sound//CSoundManager.h" // サウンドマネージャークラス

CCollisionManager::CCollisionManager()
	: m_pStaticBlast()

	// 壁
	, m_pWallTop()
	, m_pWallBottom()
	, m_pWallLeft()
	, m_pWallRight()

	// 地面
	, m_pGround()

	// 木箱
	, m_pWoodBoxTopLeft()
	, m_pWoodBoxTopRight()
	, m_pWoodBoxCenter()
	, m_pWoodBoxBottomLeft()
	, m_pWoodBoxBottomRight()

	// 弾マネージャー
	, m_pShotManager()

	// キャラクターマネージャー
	, m_pCharacterManager()

	// アイテムボックスマネージャー
	, m_pItemBoxManager()

	//爆風マネージャー.
	, m_pBlastManager()

	, m_Speed(15.0f)
{
}

CCollisionManager::~CCollisionManager()
{
}

void CCollisionManager::Update()
{
	// 壁とプレイヤーの当たり判定
	WalltoCharacter();

	// 壁と弾の当たり判定
	WalltoShot();

	// プレイヤーとプレイヤー当たり判定判別
	CharactertoCharacter();

	//// キャラクターとアイテムボックス
	//CharactertoItemBox();

	// プレイヤーと弾
	CharactertoShot();

	// 箱とプレイヤー
	WoodBoxtoCharacter();

	// 木箱と弾
	WoodBoxtoShot();

	// 地面と弾
	GroundtoShot();

	//// 地面とアイテムボックス
	//GroundtoItemBox();

	//復活するかも.
	////アイテムと木箱.
	//ItemtoWoodBox();

	//爆風とプレイヤーの当たり判定.
	CharactertoBlast();
}

void CCollisionManager::WalltoCharacter()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
#if 0
		bool isHitWall = false; // 壁と衝突しているか?

		// 押し返しの強さ
		const float pushStrength = m_pCharacterManager->GetTuning(i).moveSpeed;

		// i 番のプレイヤーを取得
		auto chara = m_pCharacterManager->GetControlPlayer(i);
		if (!chara)continue;
		auto Coll = chara->GetBody()->GetCollider();

		// 押し返すための変数
		D3DXVECTOR3 push(0.0f, 0.0f, 0.0f);

		// 車体が壁と接触したとき
		if (Coll->CheckCollision(*m_pWallTop->GetCollider()) &&
			chara->GetDeath() == false)
		{
			push.z -= pushStrength;
			isHitWall = true;
		}

		if (Coll->CheckCollision(*m_pWallBottom->GetCollider()) &&
			chara->GetDeath() == false)
		{
			push.z += pushStrength;
			isHitWall = true;
		}
		
		if (Coll->CheckCollision(*m_pWallLeft->GetCollider()) &&
			chara->GetDeath() == false)
		{
			push.x += pushStrength;
			isHitWall = true;
		}

		if (Coll->CheckCollision(*m_pWallRight->GetCollider()) &&
			chara->GetDeath() == false)
		{
			push.x -= pushStrength;
			isHitWall = true;
		}

		//// 押し返しを正規化
		//if (D3DXVec3Length(&push) > 0.f)
		//{
		//	D3DXVec3Normalize(&push, &push);
		//	push *= pushStrength;
		//}

		if (isHitWall == true)
		{
			if (chara->GetHitWall() == false)
			{
				//衝突SEの再生.
				CSoundManager::PlaySE(CSoundManager::SE_Impact);

				// 接触時にフラグをtrueにする
				chara->SetHitWall(true);
			}
		}
		else
		{
			// 壁から離れた瞬間に戻す
			chara->SetHitWall(false);
		}

		// 壁に当たった時に押し返す
		chara->GetBody()->PushBack(push);
		chara->GetCannon()->PushBack(push);
#else
		bool isHitWall = false;

		auto chara = m_pCharacterManager->GetControlPlayer(i);
		if (!chara) continue;

		auto playerColl = chara->GetBody()->GetCollider();
		if (!playerColl) continue;

		auto playerBox =
			std::dynamic_pointer_cast<CBoxCollider>(playerColl);
		if (!playerBox) continue;

		std::shared_ptr<CCollider> Allwall[] =
		{
			m_pWallTop->GetCollider(),
			m_pWallBottom->GetCollider(),
			m_pWallLeft->GetCollider(),
			m_pWallRight->GetCollider(),
		};

		float maxPenetration = 0.0f;
		D3DXVECTOR3 bestNormal(0, 0, 0);

		for (auto& wallBase : Allwall)
		{
			auto wallBox =
				std::dynamic_pointer_cast<CBoxCollider>(wallBase);
			if (!wallBox) continue;

			auto result =
				m_pWallBottom->GetCollider()->CheckCollisionBoxDetail(*playerBox);

			if (result.Hit)
			{
				isHitWall = true;

				// 一番深い衝突だけ採用
				if (result.Penetration > maxPenetration)
				{
					maxPenetration = result.Penetration;
					bestNormal = result.Normal;
				}
			}
		}

		if (isHitWall)
		{
			constexpr float SLOP = 0.001f; // 微小誤差対策

			auto pos = chara->GetPosition();
			pos += bestNormal * (maxPenetration + SLOP);
			chara->GetBody()->SetPosition(pos);
		}
#endif
	}

}

// 壁と弾の当たり判定
void CCollisionManager::WalltoShot()
{
	for (auto& shot : m_pShotManager->GetShot())
	{
		//壁をまとめて管理.
		std::shared_ptr<CCollider> Allwall[] = {
			m_pWallTop->GetCollider(),
			m_pWallBottom->GetCollider(),
			m_pWallLeft->GetCollider(),
			m_pWallRight->GetCollider(),
		};

		for (auto& wall : Allwall)
		{
			if (shot->GetCollider()->CheckCollision(*wall))
			{
				//爆風の動的生成.
				m_pBlastManager->Create(
					shot->GetPosition(),
					m_pStaticBlast,
					m_Speed,
					shot->GetPlayerID());

				shot->HitShot();
			}
		}
		{
			//// 壁が弾と接触したとき
			//if (shot->GetCollider()->CheckCollision(*m_pWallTop->GetCollider()))
			//{
			//	//動的に作成.
			//	m_pBlastManager->Create(
			//		shot->GetPosition(),
			//		true,
			//		m_pStaticBlast);

			//	m_pBlastManager->SetBlastRadiusMax(m_Rad.front());

			//	m_pShotManager->HitShot();
			//}
			//if (shot->GetCollider()->CheckCollision(*m_pWallBottom->GetCollider()))
			//{
			//	//動的に作成.
			//	m_pBlastManager->Create(
			//		shot->GetPosition(),
			//		true,
			//		m_pStaticBlast);

			//	m_pBlastManager->SetBlastRadiusMax(m_Rad.front());

			//	m_pShotManager->HitShot();
			//}
			//if (shot->GetCollider()->CheckCollision(*m_pWallLeft->GetCollider()))
			//{
			//	//動的に作成.
			//	m_pBlastManager->Create(
			//		shot->GetPosition(),
			//		true,
			//		m_pStaticBlast);

			//	m_pBlastManager->SetBlastRadiusMax(m_Rad.front());

			//	m_pShotManager->HitShot();
			//}
			//if (shot->GetCollider()->CheckCollision(*m_pWallRight->GetCollider()))
			//{
			//	//動的に作成.
			//	m_pBlastManager->Create(
			//		shot->GetPosition(),
			//		true,
			//		m_pStaticBlast);

			//	m_pBlastManager->SetBlastRadiusMax(m_Rad.front());

			//	m_pShotManager->HitShot();
			//}
		}
	}
}

// プレイヤーとプレイヤー当たり判別
void CCollisionManager::CharactertoCharacter()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		//押し返す力.
		const float pushStrength = m_pCharacterManager->GetTuning(i).moveSpeed;

		// プレイヤーAのコライダー取得
		auto charaA = m_pCharacterManager->GetControlPlayer(i);
		if (!charaA)continue;
		auto CollA = charaA->GetBody()->GetCollider();

		for (int j = 0; j < PLAYER_MAX; j++)
		{
			// 自分自身との判定をスキップ
			if (i == j) continue;

			// プレイヤーBのコライダー取得
			auto charaB = m_pCharacterManager->GetControlPlayer(j);
			if (!charaB->IsPlayer())continue;
			auto CollB = charaB->GetBody()->GetCollider();

			if (CollA->CheckCollision(*CollB))
			{
				// 衝突時の押し返し処理例
				D3DXVECTOR3 push = charaA->GetBody()->GetPosition() - charaB->GetBody()->GetPosition();

				// pushベクトルを正規化して押し返しの強さをかける
				float length = D3DXVec3Length(&push);
				if (length > 0.0001f)
				{
					push /= length;
					push *= pushStrength;
					charaA->GetBody()->PushBack(push);
				}
			}
		}
	}
}

// キャラクターとアイテムボックス
void CCollisionManager::CharactertoItemBox()
{
	for (int PlayerIndex = 0; PlayerIndex < PLAYER_MAX; ++PlayerIndex)
	{
		// i 番のプレイヤーを取得
		auto chara = m_pCharacterManager->GetControlPlayer(PlayerIndex);
		if (!chara)continue;
		auto Coll = chara->GetBody()->GetCollider();

		for (size_t ItemIndex = 0; ItemIndex < m_pItemBoxManager->GetItem().size(); ItemIndex++)
		{
			// プレイヤーがアイテムと接触したとき
			if (Coll->CheckCollision(*m_pItemBoxManager->GetItem()[ItemIndex]->GetCollider()))
			{
				//アイテムの中を設定してあげる.
				m_pItemBoxManager->SetItemInfo(ItemIndex);
				//画面から消す.
				m_pItemBoxManager->GetItem()[ItemIndex]->HitPlayer();

				SetItemInfomation(ItemIndex, PlayerIndex);

			}
		}
	}
}

// プレイヤーと弾
void CCollisionManager::CharactertoShot()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// プレイヤーのコライダー取得
		auto chara = m_pCharacterManager->GetControlPlayer(i);
		if (!chara)continue;
		auto Coll = chara->GetBody()->GetCollider();

		for (auto& shot : m_pShotManager->GetShot())
		{
			if (shot->GetCollider()->CheckCollision(*Coll) && chara->GetDeath() == false)
			{
				//爆風の動的生成.
				m_pBlastManager->Create(
					shot->GetPosition(),
					m_pStaticBlast,
					m_Speed,
					shot->GetPlayerID());

				shot->HitShot();
			}
		}
	}
}

// 木箱とプレイヤー
void CCollisionManager::WoodBoxtoCharacter()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// 押し返しの強さ
		const float pushStrength = m_pCharacterManager->GetTuning(i).moveSpeed;

		// i 番のプレイヤーを取得
		auto chara = m_pCharacterManager->GetControlPlayer(i);
		if (!chara)continue;
		auto charaColl = chara->GetBody()->GetCollider();


		//木箱をまとめて管理.
		std::shared_ptr<CCollider> woodbox[] = {
			m_pWoodBoxBottomLeft->GetCollider(),
			m_pWoodBoxBottomRight->GetCollider(),
			m_pWoodBoxCenter->GetCollider(),
			m_pWoodBoxTopLeft->GetCollider(),
			m_pWoodBoxTopRight->GetCollider()
		};

		for (auto& box : woodbox)
		{
			if (charaColl->CheckCollision(*box) &&
				chara->GetDeath() == false)
			{
				//キャラの位置.
				D3DXVECTOR3 charaPos = chara->GetBody()->GetPosition();

				//方向ベクトル.
				D3DXVECTOR3 dir = charaPos - box->GetPosition();

				//長さを計算.
				float len = D3DXVec3Length(&dir);

				if (len > 0.0001f)
				{
					//正規化.
					D3DXVec3Normalize(&dir, &dir);
					dir *= pushStrength;

					// 壁に当たった時に押し返す
					chara->GetBody()->PushBack(dir);
					chara->GetCannon()->PushBack(dir);

					if (chara->GetHitBox() == false)
					{
						//衝突SEの再生.
						CSoundManager::PlaySE(CSoundManager::SE_Impact);

						// 接触時にフラグをtrueにする
						chara->SetHitBox(true);
					}
				}
			}
		}
	}
}



// 木箱と弾
void CCollisionManager::WoodBoxtoShot()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		for (auto& shot : m_pShotManager->GetShot())
		{
			//木箱をまとめて管理.
			std::shared_ptr<CCollider> woodbox[] = {
				m_pWoodBoxBottomLeft->GetCollider(),
				m_pWoodBoxBottomRight->GetCollider(),
				m_pWoodBoxCenter->GetCollider(),
				m_pWoodBoxTopLeft->GetCollider(),
				m_pWoodBoxTopRight->GetCollider()
			};

			for (auto& box : woodbox)
			{
				if (shot->GetCollider()->CheckCollision(*box))
				{
					//爆風の動的生成.
					m_pBlastManager->Create(
						shot->GetPosition(),
						m_pStaticBlast,
						m_Speed,
						shot->GetPlayerID());

					shot->HitShot();
				}
			}
			{
				//// 壁が弾と接触したとき
				//if (shot->GetCollider()->CheckCollision(*m_pWoodBoxTopLeft->GetCollider()))
				//{
				//	// 動的に作成
				//	m_pBlastManager->Create(
				//		shot->GetPosition(),
				//		true,
				//		m_pStaticBlast);

				//	m_pBlastManager->SetBlastRadiusMax(m_Rad.front());

				//	shot->HitShot();
				//}
				//if (shot->GetCollider()->CheckCollision(*m_pWoodBoxTopRight->GetCollider()))
				//{
				//	//動的に作成.
				//	m_pBlastManager->Create(
				//		shot->GetPosition(),
				//		true,
				//		m_pStaticBlast);

				//	m_pBlastManager->SetBlastRadiusMax(m_Rad.front());

				//	shot->HitShot();
				//}
				//if (shot->GetCollider()->CheckCollision(*m_pWoodBoxCenter->GetCollider()))
				//{
				//	//動的に作成.
				//	m_pBlastManager->Create(
				//		shot->GetPosition(),
				//		true,
				//		m_pStaticBlast);

				//	m_pBlastManager->SetBlastRadiusMax(m_Rad.front());

				//	shot->HitShot();
				//}
				//if (shot->GetCollider()->CheckCollision(*m_pWoodBoxBottomLeft->GetCollider()))
				//{
				//	//動的に作成.
				//	m_pBlastManager->Create(
				//		shot->GetPosition(),
				//		true,
				//		m_pStaticBlast);

				//	m_pBlastManager->SetBlastRadiusMax(m_Rad.front());

				//	shot->HitShot();
				//}
				//if (shot->GetCollider()->CheckCollision(*m_pWoodBoxBottomRight->GetCollider()))
				//{
				//	//動的に作成.
				//	m_pBlastManager->Create(
				//		shot->GetPosition(),
				//		true,
				//		m_pStaticBlast);

				//	m_pBlastManager->SetBlastRadiusMax(m_Rad.front());

				//	shot->HitShot();
				//}
			}
		}
	}
}

// 地面と弾
void CCollisionManager::GroundtoShot()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		for (auto& shot : m_pShotManager->GetShot())
		{
			if (shot->GetCollider()->CheckCollision(*m_pGround->GetCollider()) && shot->GetShotFlag() == true)
			{
				//爆風の動的生成.
				m_pBlastManager->Create(
					shot->GetPosition(),
					m_pStaticBlast,
					m_Speed,
					shot->GetPlayerID());

				shot->HitShot();
			}
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
			item->StartEffect();
		}
	}
}

// 爆風とプレイヤーの当たり判定
void CCollisionManager::CharactertoBlast()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		//i番目のプレイヤーを取得.
		auto chara = m_pCharacterManager->GetControlPlayer(i);
		if (!chara)continue;
		auto Coll = chara->GetBody()->GetCollider();

		for (auto& blast : m_pBlastManager->GetAllBlast())
		{
			// 自身の爆風には当たらないようにする
			if (blast->GetPlayerID() == chara->GetPlayerID())
			{
				continue;
			}

			if (blast->GetCollider()->CheckCollision(*Coll) && chara->GetDeath() == false)
			{
				// 当たった時の処理
				chara->Damage();

				if (chara->GetHitBlast() == false)
				{
					// ダメージSEの再生
					CSoundManager::PlaySE(CSoundManager::SE_Damage);

					// 接触中にフラグをtrueに変更
					chara->SetHitBlast(true);
				}
			}

			if (chara->GetDeath() == true && chara->GetKill() == false)
			{
				//倒した数を増やす.
				CGameDataManager::GetInstance().AddKillCount(blast->GetPlayerID(), 1);

				//各プレイヤーのキル数表示.
				std::cout << "各プレイヤーのキル数表示" << blast->GetPlayerID() << ":" << CGameDataManager::GetInstance().GetKillCount(blast->GetPlayerID()) << std::endl << std::endl << std::endl;

				chara->SetKill(true);
			}
		}
	}
}

//アイテムと木箱.
void CCollisionManager::ItemtoWoodBox()
{
	//アイテム一つ一つの情報.
	auto item = m_pItemBoxManager->GetItem();

	//木箱とかぶった時のアイテム保持用.
	std::vector<int> DeleteIndex;

	//木箱をまとめて管理.
	std::shared_ptr<CCollider> woodbox[] = {
		m_pWoodBoxBottomLeft->GetCollider(),
		m_pWoodBoxBottomRight->GetCollider(),
		m_pWoodBoxCenter->GetCollider(),
		m_pWoodBoxTopLeft->GetCollider(),
		m_pWoodBoxTopRight->GetCollider()
	};

	//当たり判定.
	for (size_t i = 0; i < item.size(); i++)
	{
		//アイテムの当たり判定.
		auto itemcol = m_pItemBoxManager->GetCollider(i);

		for (auto& box : woodbox)
		{
			//各アイテムと各木箱の当たり判定.
			if (itemcol->CheckCollision(*box))
			{
				DeleteIndex.push_back(i);
				break;
			}
		}
	}

	for (size_t i = 0; i < DeleteIndex.size(); i++)
	{
		m_pItemBoxManager->RemoveItem(DeleteIndex[i]);
		m_pItemBoxManager->Create();
	}
}

//アイテムの設定.
void CCollisionManager::SetItemInfomation(int Itemindex, int Playerindex)
{
#define ENABLE_ITEMS
	//無敵処理.
	//プレイヤーに設定.
	if (m_pItemBoxManager->GetItemInfo(Itemindex).m_ShieldFlag == true)
	{

	}

	//速度設定.
	//プレイヤーに設定.
	if (m_pItemBoxManager->GetItemInfo(Itemindex).m_Speed > 0.0f)
	{
		//プレイヤーの速度を設定.
		const TankTuning Info = { m_pItemBoxManager->GetItemInfo(Itemindex).m_Speed, 0.03f, 0.03f, 0.3f };
		//プレイヤーの情報を設定.
		m_pCharacterManager->SetPlayerTuning(Playerindex, Info);

	}

	//攻撃力設定.
	//弾に設定.
	if (m_pItemBoxManager->GetItemInfo(Itemindex).m_Power > 0.0f)
	{

	}

	//爆風の半径設定.
	//爆風に設定.
	if (m_pItemBoxManager->GetItemInfo(Itemindex).m_Blast > 0.0f)
	{
		//m_pCharacterManager->SetBlastFlag(Playerindex, m_pItemBoxManager->GetItemInfo(Itemindex).m_Blast);
	}

	//装填時短設定.
	//弾に設定.
	if (m_pItemBoxManager->GetItemInfo(Itemindex).m_Reload > 0.0f)
	{

	}

	//配列(メモリ上)から消す.
	m_pItemBoxManager->RemoveItem(Itemindex);

}
