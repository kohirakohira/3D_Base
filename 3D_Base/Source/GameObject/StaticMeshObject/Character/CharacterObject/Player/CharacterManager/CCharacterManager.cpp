#include "CCharacterManager.h"

#undef max;
#undef min;

CCharacterManager::CCharacterManager()
	: offset	(20.f)
	, AngleY	(45.f)
	, m_pPlayers()
	, m_ActivePlayerIndex(0)
{
}

//PlayerがCComPlayerならそのポインタにキャストして返す.そうでなければnullptr
static inline bool IsCom(const std::shared_ptr<CPlayer>& player)
{
	return std::dynamic_pointer_cast<const CComPlayer>(player) != nullptr;
}

//プレイヤーの操作かどうかの判定
static inline bool IsHumanControlled(const std::shared_ptr<CPlayer>& player)
{
	if (auto com = std::dynamic_pointer_cast<CComPlayer>(player)) {
		return player->HasControl();   //COMでも無効ならプレイヤー操作
	}
}

CCharacterManager::~CCharacterManager()
{
}

//=======初期化======
void CCharacterManager::Init()
{
	m_pPlayers.clear();
	m_pPlayers.reserve(PLAYER_MAX);

	//BodyCannonまだ未設定
	m_pBody = nullptr;
	m_pCannon = nullptr;

	//for (int i = 0; i < PLAYER_MAX; ++i)
	//{
	//	// 各プレイヤー / COM に対応するメッシュを渡す
	//	
	//	m_pCharacterManager->CreateBounding(i, bodyMesh[i], cannonMesh[i]);
	//	m_pCharacterManager->CreateCollider(i);
	//}


	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		// コントローラーの接続状態を確認
		CController* ctrl = CControllerManager::GetInstance().GetController(i);
		bool connected = false;
		if (ctrl != nullptr)
		{
			connected = ctrl->CheckConnected();
		}

		if (connected)
		{
			//==============================
			// Padありプレイヤー生成
			//==============================

			//プレイヤー.
			auto player = std::make_shared<CPlayer>();   // インスタンス生成.
			player->Init(i);                             // 車体・砲塔を生成.
			player->SetControllerIndex(i);               // コントローラー設定.
			player->SetHasControl(true);                 // コントローラー操作ON.
			player->SetKeyBoadEnble(true);               // キーボード操作ON.

			//1人目のBodyCannonをテンプレとして控えておく
			if (!m_pBody || !m_pCannon)
			{
				SetBodyAndCannon(player->GetBody(), player->GetCannon());
			}

			m_pPlayers.push_back(player);
		}
		else
		{
			//==============================
			// COM生成
			//==============================

			auto com = std::make_shared<CComPlayer>();   //インスタンス生成.
			com->Create(i);                              //車体・砲塔を生成.
			com->SetComEnabled(true);                    //COMモードON.
			com->SetHasControl(false);                   //プレイヤー操作OFF.

			//1体目のBody/Cannon をテンプレとして控えておく
			if (!m_pBody || !m_pCannon)
			{
				SetBodyAndCannon(com->GetBody(), com->GetCannon());
			}

			m_pPlayers.push_back(com);
		}
	}

	//COMプレイヤー同士で参照を共有.
	for (auto& player : m_pPlayers)
	{
		if (auto com = std::dynamic_pointer_cast<CComPlayer>(player))
		{
			com->SetPlayersRef(&m_pPlayers);

		}
	}

	SetStartPosition();

	//アクティブプレイヤーは最初に見つかった人間プレイヤーにしておく
	m_ActivePlayerIndex = 0;
	for (int i = 0; i < (int)m_pPlayers.size(); ++i)
	{
		if (auto p = std::dynamic_pointer_cast<CPlayer>(m_pPlayers[i]))
		{
			if (p->HasControl())
			{
				m_ActivePlayerIndex = i;
				break;
			}
		}
	}

#if 0
	// 誰が何番に入ったか確認したい時用のログ
	for (int i = 0; i < (int)m_pPlayers.size(); ++i)
	{
		bool isCom = (std::dynamic_pointer_cast<CComPlayer>(m_pPlayers[i]) != nullptr);

		auto body = m_pPlayers[i]->GetBody();
		auto cannon = m_pPlayers[i]->GetCannon();
		D3DXVECTOR3 pos(0, 0, 0);
		if (body) pos = body->GetPosition();

		printf("Init Slot %d : %s  player=%p body=%p cannon=%p pos=(%.1f, %.1f, %.1f)\n",
			i,
			isCom ? "COM" : "PLAYER",
			(void*)m_pPlayers[i].get(),
			(void*)(body ? body.get() : nullptr),
			(void*)(cannon ? cannon.get() : nullptr),
			pos.x, pos.y, pos.z
		);
	}
#endif
}
//===================

//=======更新=======
void CCharacterManager::Update()
{
	//pad の接続状態に応じてPlayer.COMを入れ替える
	SwitchControl();

	const int count = static_cast<int>(m_pPlayers.size());
	if (count <= 0) return;

	//人間が操作しているプレイヤーを1人決める
	auto pickHumanTargetIndex = [&]() -> int
		{
			auto isValidHuman = [&](int idx) -> bool
				{
					if (idx < 0 || idx >= count) return false;
					auto& p = m_pPlayers[idx];
					if (!p) return false;
					return p->HasControl();
				};

			if (isValidHuman(m_LockTargetIndex))   return m_LockTargetIndex;
			if (isValidHuman(m_ActivePlayerIndex)) return m_ActivePlayerIndex;

			for (int i = 0; i < count; ++i)
			{
				if (isValidHuman(i)) return i;
			}
			return -1;
		};

	const int tgtIdx = pickHumanTargetIndex();
	auto target = (tgtIdx >= 0) ? m_pPlayers[tgtIdx] : nullptr;

	//各スロットごとにUpdate
	for (int i = 0; i < count; ++i)
	{
		auto self = m_pPlayers[i];
		if (!self) continue;

		//COMかどうか判定
		if (auto com = std::dynamic_pointer_cast<CComPlayer>(self))
		{
			if (com->IsComEnabled())
			{
				//COMが追いかけるターゲットを設定
				if (target && target != self)
				{
					com->SetTarget(target);    // プレイヤーを追いかける
				}
				else
				{
					com->ClearTarget();
				}

				//AIとしてのUpdate
				com->Update();
			}
			else
			{
				//COMインスタンスだけど人間操作モードの場合
				self->Update();
			}
		}
		else
		{
			//素のPlayerなどは普通にUpdate
			self->Update();
		}
	}
}
//==================

//=======描画=======
void CCharacterManager::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	for (auto& player : m_pPlayers)
	{
		player->Draw(View, Proj, Light, Camera);
	}
}
//==================

//=======メッシュをアタッチ=======
void CCharacterManager::AttachMeshesToPlayer(int index, std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon)
{
	m_pPlayers[index]->AttachMeshse(pBody, pCannon);
}
//==============================

//=======プレイヤー座標設定=======
void CCharacterManager::SetPlayerPosition(int index, const D3DXVECTOR3& pos)
{
	if (index < m_pPlayers.size())
	{
		m_pPlayers[index]->SetTankPosition(pos);
	}
}
//==============================

//=======プレイヤー回転設定=======
void CCharacterManager::SetPlayerRotation(int index, const D3DXVECTOR3& rad)
{
	if (index < m_pPlayers.size())
	{
		m_pPlayers[index]->SetTankRotation(rad);
	}
}
//==============================

//=======プレイヤー拡縮設定=======
void CCharacterManager::SetPlayerScale(int index, const D3DXVECTOR3& xyz)
{
	if (index < m_pPlayers.size())
	{
		m_pPlayers[index]->SetTankScale(xyz);
	}
}
//==============================

//=======バウンディングの作成=======
void CCharacterManager::CreateBounding(int index, const std::shared_ptr<CStaticMesh>& body, const std::shared_ptr<CStaticMesh>& cannon)
{
	if (index < m_pPlayers.size())
	{
		//デバッグログ
		printf("[CreateBounding] index=%d player=%p bodyMesh=%p cannonMesh=%p\n",
			index,
			(void*)m_pPlayers[index].get(),
			(void*)m_pBody.get(),
			(void*)m_pCannon.get());

		m_pPlayers[index]->SetBounding(body, cannon);
	}
}
//================================

//=======コライダーの作成=======
void CCharacterManager::CreateCollider(int index)
{
	if (index < m_pPlayers.size())
	{
		//デバッグ
		printf("[CreateCollider] index=%d player=%p\n",
			index,
			(void*)m_pPlayers[index].get());

		m_pPlayers[index]->CreateCollider();
	}
}
//============================

//=======プレイヤーのリスポーン=======
void CCharacterManager::PlayerRespawn(int index)
{
	if (index < m_pPlayers.size())
	{
		if (m_pPlayers[index]->GetRespawnFlag() == true)
		{
			SetRespawnArea(index);
			m_pPlayers[index]->SetRespawnFlag(false);
		}
	}
}
//==================================

//=======リスポーン座標設定=======
void CCharacterManager::SetRespawnArea(int index)
{
	// エリア4つを定義（マップの座標系に合わせて調整）
	// リスポーン位置をセット
	areas[0].RespawnPos = { -offset, 0.f,  offset }; // 左上
	areas[1].RespawnPos = {  offset, 0.f,  offset }; // 右上
	areas[2].RespawnPos = { -offset, 0.f, -offset }; // 左下
	areas[3].RespawnPos = {  offset, 0.f, -offset }; // 右下

	// リスポーン向きをセット
	areas[0].RespawnRot = { 0.0f, D3DXToRadian(AngleY * 3), 0.0f }; // 左上
	areas[1].RespawnRot = { 0.0f, D3DXToRadian(AngleY * 5), 0.0f }; // 右上
	areas[2].RespawnRot = { 0.0f, D3DXToRadian(AngleY),	    0.0f }; // 左下
	areas[3].RespawnRot = { 0.0f, D3DXToRadian(AngleY * 7), 0.0f }; // 右下

	// 各プレイヤーがどのエリアにいるか調べる
	for (int index = 0; index < PLAYER_MAX; index++)
	{
		auto PPos = m_pPlayers[index]->GetBody()->GetPosition();

		int areaIndex = GetAreaIndex(PPos.x, PPos.z);

		areas[areaIndex].Taken = true;
	}

	// 空いているエリアを探す
	int freeIndex = -1;
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		if (areas[i].Taken == false)
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
	if (index < m_pPlayers.size())
	{
		m_pPlayers[index]->SetTankPosition(areas[freeIndex].RespawnPos);
		m_pPlayers[index]->SetTankRotation(areas[freeIndex].RespawnRot);

		for (int index = 0; index < PLAYER_MAX; index++)
		{
			areas[index].Taken = false;
		}
	}
}
//==============================

//=======中央を跨がないように計算=======
int CCharacterManager::GetAreaIndex(float x, float z)
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
//====================================

//=======ゲーム開始時の座標設定=======
//void CCharacterManager::SetStartPosition()
//{
//	for (int index = 0; index < PLAYER_MAX; ++index)
//	{
//		if (index == 0)	
//		{
//			// 座標を設定
//			m_pPlayers[index]->SetTankPosition(D3DXVECTOR3(-offset, 0.0f, -offset));
//			// 回転を設定
//			m_pPlayers[index]->SetTankRotation(D3DXVECTOR3(0.f, D3DXToRadian(AngleY), 0.f));
//			// スケールを設定
//			m_pPlayers[index]->SetTankScale(D3DXVECTOR3(1.8f, 1.8f, 1.8f));
//		}
//		else if (index == 1)
//		{
//			// 座標を設定
//			m_pPlayers[index]->SetTankPosition(D3DXVECTOR3(-offset, 0.0f, offset));
//			// 回転を設定
//			m_pPlayers[index]->SetTankRotation(D3DXVECTOR3(0.f, D3DXToRadian(AngleY * 3), 0.f));
//			// スケールを設定
//			m_pPlayers[index]->SetTankScale(D3DXVECTOR3(1.8f, 1.8f, 1.8f));
//		}
//		else if (index == 2)
//		{
//			// 座標を設定
//			m_pPlayers[index]->SetTankPosition(D3DXVECTOR3(offset, 0.0f, offset));
//			// 回転を設定
//			m_pPlayers[index]->SetTankRotation(D3DXVECTOR3(0.f, D3DXToRadian(AngleY * 5), 0.f));
//			// スケールを設定
//			m_pPlayers[index]->SetTankScale(D3DXVECTOR3(1.8f, 1.8f, 1.8f));
//		}
//		else if (index == 3)
//		{
//			// 座標を設定
//			m_pPlayers[index]->SetTankPosition(D3DXVECTOR3(offset, 0.0f, -offset));
//			// 回転を設定
//			m_pPlayers[index]->SetTankRotation(D3DXVECTOR3(0.f, D3DXToRadian(AngleY * 7), 0.f));
//			// スケールを設定
//			m_pPlayers[index]->SetTankScale(D3DXVECTOR3(1.8f, 1.8f, 1.8f));
//		}
//	}
//
//}

//=======ゲーム開始時の座標設定=======
void CCharacterManager::SetStartPosition()
{
	const int count = (int)m_pPlayers.size();
	for (int index = 0; index < count; ++index)
	{
		if (!m_pPlayers[index]) continue;

		D3DXVECTOR3 pos;
		D3DXVECTOR3 rot;
		D3DXVECTOR3 sca;

		if (index == 0)
		{
			pos = D3DXVECTOR3(-offset, 0.0f, -offset);
			rot = D3DXVECTOR3(0.f, D3DXToRadian(AngleY), 0.f);
			sca = D3DXVECTOR3(1.8f, 1.8f, 1.8f);
		}
		else if (index == 1)
		{
			pos = D3DXVECTOR3(-offset, 0.0f, offset);
			rot = D3DXVECTOR3(0.f, D3DXToRadian(AngleY * 3), 0.f);
			sca = D3DXVECTOR3(1.8f, 1.8f, 1.8f);
		}
		else if (index == 2)
		{
			pos = D3DXVECTOR3(offset, 0.0f, offset);
			rot = D3DXVECTOR3(0.f, D3DXToRadian(AngleY * 5), 0.f);
			sca = D3DXVECTOR3(1.8f, 1.8f, 1.8f);
		}
		else if (index == 3)
		{
			pos = D3DXVECTOR3(offset, 0.0f, -offset);
			rot = D3DXVECTOR3(0.f, D3DXToRadian(AngleY * 7), 0.f);
			sca = D3DXVECTOR3(1.8f, 1.8f, 1.8f);
		}
		else
		{
			continue;
		}

		//戦車全体の位置・回転
		m_pPlayers[index]->SetTankPosition(pos);
		m_pPlayers[index]->SetTankRotation(rot);
		m_pPlayers[index]->SetTankScale(D3DXVECTOR3(1.8f, 1.8f, 1.8f));

		//BodyCannonにも直接書き込む
		if (auto body = m_pPlayers[index]->GetBody())
		{
			body->SetPosition(pos);
			body->SetRotation(rot);
		}

		if (auto cannon = m_pPlayers[index]->GetCannon())
		{
			D3DXVECTOR3 cpos = pos;
			cannon->SetPosition(cpos);

			D3DXVECTOR3 crot = cannon->GetRotation();
			crot.y = rot.y;
			cannon->SetRotation(crot);
		}
	}

}

//=======プレイヤーを取得=======	
//std::shared_ptr<CCharacterObjectBase> CCharacterManager::GetControlPlayer(int index)
//{
//
//	for (const auto& p : m_pPlayers)
//	{
//		if (p && p->HasControl())
//			return p;
//	}
//	return nullptr;
//}

//=======プレイヤーを取得=======	
std::shared_ptr<CCharacterObjectBase> CCharacterManager::GetControlPlayer(int index)
{
	if (index >= 0 && index < static_cast<int>(m_pPlayers.size()))
	{
		return m_pPlayers[index];
	}
	return nullptr;
}


//============================

void CCharacterManager::SwitchActivePlayer()
{
	if (m_pPlayers.empty()) return;	//0人ならなにもしない

	const int prev = m_ActivePlayerIndex;	//直前に操作してたやつ
	const int next = (m_ActivePlayerIndex + 1) % (int)m_pPlayers.size();	//次の対象にする

	//全員の操作権を落とす
	for (auto& p : m_pPlayers) p->SetHasControl(false);

	//前のアクティブがCOMなら戻す
	if (prev >= 0)
	{
		if (auto prevCom = std::dynamic_pointer_cast<CComPlayer>(m_pPlayers[prev]))
		{
			prevCom->SetComEnabled(true);	//COM操作
		}
	}
	//次のアクティブがCOMならプレイヤー操作に切り替える
	if (auto nextCom = std::dynamic_pointer_cast<CComPlayer>(m_pPlayers[next])) {
		nextCom->SetComEnabled(false);	//プレイヤー操作
	}
	//次のやつに操作権を渡す
	m_pPlayers[next]->SetHasControl(true);
	m_ActivePlayerIndex = next;
}

D3DXVECTOR3 CCharacterManager::GetPosition(int index) const
{
	if (index >= 0 && index < static_cast<int>(m_pPlayers.size()))
	{
		return m_pPlayers[index]->GetPosition();
	}
	return D3DXVECTOR3(0, 0, 0);
}

D3DXVECTOR3 CCharacterManager::GetRotation(int idx) const
{
	if (idx >= 0 && idx < static_cast<int>(m_pPlayers.size()))
	{
		return m_pPlayers[idx]->GetRotation();
	}
	return D3DXVECTOR3(0, 0, 0);
}

void CCharacterManager::SetShotManager(std::shared_ptr<CShotManager>& mgr)
{
	m_ShotManager = mgr;

	//すでにいる前COMに渡す　
	for (auto& up : m_pPlayers) {
		if (auto* com = dynamic_cast<CComPlayer*>(up.get())) {	//CComPlayerなら生のポインタにして渡す.所有権は渡さない
			com->AttachShotManager(m_ShotManager);	//weak_ptrに渡す
		}
	}
}

void CCharacterManager::SetBodyAndCannon(std::shared_ptr<CBody> body, std::shared_ptr<CCannon> cannon)
{
	//このクラス内で使えるようにする.
	m_pBody = body;
	m_pCannon = cannon;	
	
	
#if 0
	for (auto& player : m_pPlayers)
	{
		player->SetCBody(body);
		player->SetCannon(cannon);
	}
#endif
}

void CCharacterManager::SetPlayerTuningAll(const TankTuning& t)
{
	for (auto& player : m_pPlayers)player->SetTuning(t);
}

void CCharacterManager::SetPlayerTuning(int idx, const TankTuning& t)
{
	if (idx >= 0 && idx < (int)m_pPlayers.size())m_pPlayers[idx]->SetTuning(t);
}

//プレイヤーとCOMの自動切り替え.
void CCharacterManager::SwitchControl()
{
	for (int No = 0; No < PLAYER_MAX; No++)
	{
		//コントローラーの接続状態を確認.
		CController* ctrl = CControllerManager::GetInstance().GetController(No);
		//接続出来てる？を判定する用.
		bool Connected = false;
		//判定中.
		if (ctrl != nullptr)
		{
			Connected = ctrl->CheckConnected();
		}

		//現在のプレイヤー情報を取得.
		std::shared_ptr<CCharacter> current = nullptr;	//CCharacterObjectBase
		//プレイヤーリストの範囲内なら、その番号のプレイヤーを取得.
		if (No < static_cast<int>(m_pPlayers.size()))
		{
			current = m_pPlayers[No];
		}

		//COMかどうか判定.
		bool isCom = false;
		if (current != nullptr)
		{
			isCom = std::dynamic_pointer_cast<CComPlayer>(current) != nullptr;
		}

		//状態が一致している場合はスキップ.
		//例：padあり->プレイヤー・padなし->COMの状態があっているとき.
		if ((Connected == true && isCom != true) || (Connected != true && isCom == true))
		{
			//変更する必要なし.
			continue;
		}

		//COM->プレイヤーへ切り替え.
		if (Connected == true && isCom == true)
		{
			//新しいプレイヤーを生成.
			std::shared_ptr<CPlayer> newPlayer = std::make_shared<CPlayer>();

			//プレイヤーとしての初期化
			newPlayer->Init(No);

			//コントローラーを設定.
			newPlayer->SetControllerIndex(No);

			newPlayer->SetHasControl(true);
			newPlayer->SetKeyBoadEnble(true);

			//元のCOMの位置を引き継ぐ.
			if (current != nullptr)
			{
				newPlayer->SetPosition(current->GetPosition());
			}
			//戦車の調整データを引き継ぐ.
			if (current != nullptr)
			{
				newPlayer->SetTuning(current->GetTuning());
			}

			//車体と砲塔のインスタンスを設定.
			newPlayer->SetCBody(m_pBody);
			newPlayer->SetCannon(m_pCannon);

			//COMからプレイヤーに入れ替え.
			m_pPlayers[No] = newPlayer;
		}
		//プレイヤー->COMへ切り替え.
		else if (Connected != true && isCom != true)
		{
			//新しCOMを生成.
			std::shared_ptr<CComPlayer> newCOM = std::make_shared<CComPlayer>();

			//COMとして初期化
			newCOM->Create(No);

			//COMとして動かしたいのでAI有効と人間操作無効
			newCOM->SetComEnabled(true);
			newCOM->SetHasControl(false);

			//元のプレイヤーの位置を引き継ぐ.
			if (current != nullptr)
			{
				newCOM->SetPosition(current->GetPosition());
			}

			//COMどうしで参照を共有できるようにする
			newCOM->SetPlayersRef(&m_pPlayers);

			//すでにShotManagerが設定されていればCOMにもつける
			if (m_ShotManager)
			{
				newCOM->AttachShotManager(m_ShotManager);
			}

			//プレイヤーからCOMに入れ替え.
			m_pPlayers[No] = newCOM;
		}
	}
}

// 関数の中がプレイヤー座標と中身が同じ、この関数必要？
void CCharacterManager::SetPushBackPosision(int index, const D3DXVECTOR3& push)
{
	if (index < m_pPlayers.size())
	{
		m_pPlayers[index]->SetTankPosition(push);
	}
}
