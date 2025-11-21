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

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// コントローラーの取得
		CController* ctrl = CControllerManager::GetInstance().GetController(i);

		//オブジェクトがあるかではなく実際に接続されているかで判定する
		bool connected = false;
		if (ctrl != nullptr)
		{
			connected = ctrl->CheckConnected();
		}

		if (connected)
		{
			//=================
			//   プレイヤー
			//=================
			auto player = std::make_shared<CPlayer>();	// インスタンス生成.
			player->Init(i);							// 車体・砲塔を生成.
			player->SetHasControl(true);				// 操作権ON
			player->SetKeyBoadEnble(true);				// （内部的には m_HasControl を true にする）
			player->SetControllerIndex(i);				// コントローラー番号を教える

			// Body / Cannon のポインタをマネージャ内にキャッシュ
			SetBodyAndCannon(player->GetBody(), player->GetCannon());

			// リストに追加
			m_pPlayers.push_back(std::move(player));
		}
		else
		{
			//=================
			//      COM
			//=================
			auto com = std::make_shared<CComPlayer>();	//インスタンス生成.
			com->Create(i);								//車体・砲塔を生成.
			com->SetComEnabled(true);					// COMのAI処理を有効にするフラグ
			com->SetHasControl(false);					// 人間入力は読まない

			// Body / Cannon のポインタをマネージャ内にキャッシュ
			SetBodyAndCannon(com->GetBody(), com->GetCannon());

			// リストに追加
			m_pPlayers.push_back(std::move(com));
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
}
//===================
//===================

//=======更新=======
void CCharacterManager::Update()
{
	//コントローラーの接続状態をチェック・切り替え.
	SwitchControl();

	for (auto& player : m_pPlayers)
	{
		player->Update();
	}


	//{
	//	const int count = static_cast<int>(m_pPlayers.size());
	//	if (count <= 0)return;

	//	//基本ターゲットを決める
	//	//ラムダ式[&]で外側の変数を参照でつかう.->int戻り値のかた指定
	//	auto pickHumanTargetIndex = [&]()->int {
	//		auto isValidHuman = [&](int idx) {
	//			return(idx == m_ActivePlayerIndex) && IsHumanControlled(m_pPlayers[idx]);
	//			};
	//		if (isValidHuman(m_LockTargetIndex)) return m_LockTargetIndex;
	//		if (isValidHuman(m_ActivePlayerIndex)) return m_ActivePlayerIndex;
	//		return -1;	//プレイヤーがいない
	//		};

	//	const int tgtIdx = pickHumanTargetIndex();
	//	std::shared_ptr<CPlayer> target = (tgtIdx >= 0) ? m_pPlayers[tgtIdx] : nullptr;

	//	for (int i = 0; i < count; ++i)
	//	{
	//		auto self = m_pPlayers[i];
	//		if (auto com = std::dynamic_pointer_cast<CComPlayer>(self))
	//		{
	//			if (com->IsComEnabled())
	//			{
	//				//COM稼働中だけターゲットをあげる
	//				if (target && IsHumanControlled(target) && target != self)
	//				{
	//					//ターゲットがプレイヤー限定なのであとで消す
	//					com->SetTarget(target);
	//				}
	//				else
	//				{
	//					com->ClearTarget();
	//				}
	//				com->Update();
	//			}
	//			else
	//			{
	//				self->Update();
	//			}
	//		}
	//		else
	//		{
	//			self->Update();
	//		}				
	//	}
	//}
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
		m_pPlayers[index]->SetBounding(body, cannon);
	}
}
//================================

//=======コライダーの作成=======
void CCharacterManager::CreateCollider(int index)
{
	if (index < m_pPlayers.size())
	{
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
void CCharacterManager::SetStartPosition()
{
	for (int index = 0; index < PLAYER_MAX; ++index)
	{
		if (index == 0)	
		{
			// 座標を設定
			m_pPlayers[index]->SetTankPosition(D3DXVECTOR3(-offset, 0.0f, -offset));
			// 回転を設定
			m_pPlayers[index]->SetTankRotation(D3DXVECTOR3(0.f, D3DXToRadian(AngleY), 0.f));
			// スケールを設定
			m_pPlayers[index]->SetTankScale(D3DXVECTOR3(1.8f, 1.8f, 1.8f));
		}
		else if (index == 1)
		{
			// 座標を設定
			m_pPlayers[index]->SetTankPosition(D3DXVECTOR3(-offset, 0.0f, offset));
			// 回転を設定
			m_pPlayers[index]->SetTankRotation(D3DXVECTOR3(0.f, D3DXToRadian(AngleY * 3), 0.f));
			// スケールを設定
			m_pPlayers[index]->SetTankScale(D3DXVECTOR3(1.8f, 1.8f, 1.8f));
		}
		else if (index == 2)
		{
			// 座標を設定
			m_pPlayers[index]->SetTankPosition(D3DXVECTOR3(offset, 0.0f, offset));
			// 回転を設定
			m_pPlayers[index]->SetTankRotation(D3DXVECTOR3(0.f, D3DXToRadian(AngleY * 5), 0.f));
			// スケールを設定
			m_pPlayers[index]->SetTankScale(D3DXVECTOR3(1.8f, 1.8f, 1.8f));
		}
		else if (index == 3)
		{
			// 座標を設定
			m_pPlayers[index]->SetTankPosition(D3DXVECTOR3(offset, 0.0f, -offset));
			// 回転を設定
			m_pPlayers[index]->SetTankRotation(D3DXVECTOR3(0.f, D3DXToRadian(AngleY * 7), 0.f));
			// スケールを設定
			m_pPlayers[index]->SetTankScale(D3DXVECTOR3(1.8f, 1.8f, 1.8f));
		}
	}
}
//==================================

//=======プレイヤーを取得=======	
// 引数あり
std::shared_ptr<CCharacterObjectBase> CCharacterManager::GetControlPlayer(int index)
{
	if (index >= 0 && index < (int)m_pPlayers.size()) {
		return m_pPlayers[index];
	}
	return nullptr;
}
// 引数なし
std::shared_ptr<CCharacterObjectBase> CCharacterManager::GetControlPlayer()
{
	for (const auto& p : m_pPlayer)
		if (p && p->HasControl())   // ← ここで本当に操作中か判定
			return p;
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
	m_pPlayers[next]->SetHasControl(false);
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

	
#if 1
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
