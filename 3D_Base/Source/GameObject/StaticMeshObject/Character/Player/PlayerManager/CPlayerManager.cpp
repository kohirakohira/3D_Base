#include "CPlayerManager.h"


CPlayerManager::CPlayerManager()
	: m_pPlayers()
	, m_ActivePlayerIndex(0)
	, m_PadIndex()
{
}
//PlayerがCComPlayerならそのポインタにキャストして返す.そうでなければnullptr
static inline bool IsCom(const std::shared_ptr<CPlayer>& player) 
{
	return std::dynamic_pointer_cast< const CComPlayer>(player) != nullptr;
}

//プレイヤーの操作かどうかの判定
static inline bool IsHumanControlled(const std::shared_ptr<CPlayer>& player)
{
	if (auto com = std::dynamic_pointer_cast<CComPlayer>(player)) {
		return player->HasControl();   //COMでも無効ならプレイヤー操作
	}
}

CPlayerManager::~CPlayerManager()
{
}

void CPlayerManager::InitPads()
{
	//Padの初期化処理
}


//インスタンス生成.
void CPlayerManager::Initialize()
{

#if 1
	m_pPlayers.clear();
	m_pPlayers.reserve(PLAYER_MAX);


	for (int i = 0; i < PLAYER_MAX; ++i) {
		auto com = std::make_shared<CComPlayer>();
		com->Initialize(i);

		//既定はCOM
		com->SetComEnabled(true);
		com->SetHasControl(false);

		// 最初の1体だけ人間操作にする
		if (i == 0) {
			com->SetComEnabled(false); //COM停止,人間化
			com->SetHasControl(true);  //入力を読むのはこの個体だけ
		}
		m_pPlayers.push_back(std::move(com));
	}
#endif

	m_PadConnected.fill(false);		//最初の接続状態は無効
	m_PadIndex.fill(-1);			//最初は無効値で埋める
	m_PlayerPad.assign(m_pPlayers.size(), -1);	//サイズ取得と中身を同じ要素で埋める

	//サイズ分回してCXInputを生成する
	for (DWORD id = 0; id < m_Pads.size(); ++id)	//XInput側がDWORDなのであわせておく
	{
		m_Pads[id] = std::make_unique<CXInput>(id);
	}
	SyncByPadConnection();

#if 1
	//仮想コントローラー化
	//m_Pads[0]->TestMode(true);
	//m_Pads[0]->TestSetConnected(false); //起動時は未接続
	m_Pads[1]->TestMode(true);
	m_Pads[1]->TestSetConnected(false); //起動時は未接続
#endif
}

void CPlayerManager::AttachMeshesToPlayer(int index, std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon)
{
	m_pPlayers[index]->AttachMeshse(pBody, pCannon);
}

void CPlayerManager::SetPlayerPosition(int index, const D3DXVECTOR3& pos)
{
	if (index < m_pPlayers.size())
	{
		m_pPlayers[index]->SetTankPosition(pos);
	}
}

void CPlayerManager::SetPushBackPosision(int index, const D3DXVECTOR3& push)
{
	if (index < m_pPlayers.size())
	{
		m_pPlayers[index]->SetTankPosition(push);
	}
}

// バウンディングオブジェクトの作成
void CPlayerManager::CreateBounding(int index, const std::shared_ptr<CStaticMesh>& body, const std::shared_ptr<CStaticMesh>& cannon)
{
	if (index < m_pPlayers.size())
	{
		m_pPlayers[index]->SetBounding(body, cannon);
	}
}
// コライダーの作成
void CPlayerManager::CreateCollider(int index)
{
	if (index < m_pPlayers.size())
	{
		m_pPlayers[index]->CreateCollider();
	}
}

void CPlayerManager::SetPlayerRotation(int index, const D3DXVECTOR3& rad)
{
	if (index < m_pPlayers.size())
	{
		m_pPlayers[index]->SetTankRotation(rad);
	}
}

void CPlayerManager::Update()
{
	const int count = static_cast<int>(m_pPlayers.size());
	if (count <= 0)return;

	SyncByPadConnection();
#if 1
	static bool prevF2 = false;
	bool nowF2 = (GetAsyncKeyState(VK_F2) & 0x8000) != 0;
	if (nowF2 && !prevF2)
	{
		bool on = !m_PadConnected[1];
		m_Pads[1]->TestSetConnected(on);
	}
	prevF2 = nowF2;

	//仮想PADをwasdで動かす
	auto held = [](int vk) {return (GetAsyncKeyState(vk) & 0x8000) != 0; };
	SHORT lx = 0;
	SHORT ly = 0;
	SHORT rx = 0;

	//左スティックWASD
	if (held('A')) lx -= 16000; if (held('D')) lx += 16000;
	if (held('W')) ly += 16000; if (held('S')) ly -= 16000;

	//右スティック
	if (held('A')) lx -= 16000; if (held('D')) lx += 16000;
	if (held('W')) ly += 16000; if (held('S')) ly -= 16000;

	if (held(VK_LEFT)) rx -= 16000;
	if (held(VK_RIGHT)) rx += 16000;
	m_Pads[1]->TestSetStick(lx, ly, rx, 0);

#endif
#if 0
	static bool prevF3 = false;
	bool nowF3 = (GetAsyncKeyState(VK_F3) & 0x8000) != 0;
	if (nowF3 && !prevF3)
	{
		bool oncom = !m_PadConnected[1];
		m_Pads[1]->TestSetConnected(oncom);
	}
	prevF2 = nowF2;

	//仮想PADをwasdで動かす
	auto held2 = [](int vk) {return (GetAsyncKeyState(vk) & 0x8000) != 0; };
	SHORT lx2 = 0;
	SHORT ly2 = 0;
	SHORT rx2 = 0;

	//左スティックWASD
	if (held2('A')) lx2 -= 16000; if (held2('D')) lx2 += 16000;
	if (held2('W')) ly2 += 16000; if (held2('S')) ly2 -= 16000;

	//右スティック
	if (held2('A')) lx2 -= 16000; if (held2('D')) lx2 += 16000;
	if (held2('W')) ly2 += 16000; if (held2('S')) ly2 -= 16000;

	if (held2(VK_LEFT)) rx2 -= 16000;
	if (held2(VK_RIGHT)) rx2 += 16000;
	m_Pads[1]->TestSetStick(lx2, ly2, rx2, 0);
#endif

	//基本ターゲットを決める
	//ラムダ式[&]で外側の変数を参照でつかう.->int戻り値のかた指定
	auto pickHumanTargetIndex = [&]()->int {
		auto isValidHuman = [&](int idx) {
			return(idx == m_ActivePlayerIndex) && IsHumanControlled(m_pPlayers[idx]);
			};
		if (isValidHuman(m_LockTargetIndex)) return m_LockTargetIndex;
		if (isValidHuman(m_ActivePlayerIndex)) return m_ActivePlayerIndex;
		return -1;	//プレイヤーがいない
		};

	const int tgtIdx = pickHumanTargetIndex();
	std::shared_ptr<CPlayer> target = (tgtIdx >= 0) ? m_pPlayers[tgtIdx] : nullptr;

	for (int i = 0; i < count; ++i)
	{
		auto self = m_pPlayers[i];

		if (auto com = std::dynamic_pointer_cast<CComPlayer>(self))
		{
			if (com->IsComEnabled())
			{
				//COM稼働中だけターゲットをあげる
				if (target && IsHumanControlled(target) && target != self)
				{
					com->SetTarget(target);
				}
				else
				{
					com->ClearTarget();
				}
				com->Update();
			}
			else
			{
				self->Update();
			}
		}
		else
		{
			self->Update();
		}
			
	}

}

void CPlayerManager::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	for (auto& player : m_pPlayers)
	{
		player->Draw(View, Proj, Light, Camera);
	}
}

D3DXVECTOR3 CPlayerManager::GetPosition()
{
	if (m_pPlayers.empty()) return D3DXVECTOR3(0, 0, 0);
	return m_pPlayers[m_ActivePlayerIndex]->GetPosition();
}

//↓松岡.
std::shared_ptr<CPlayer> CPlayerManager::GetControlPlayer(int index)
{
	if (index >= 0 && index < (int)m_pPlayers.size()) {
		return m_pPlayers[index];
	}
	return nullptr;
}

//↓濵口コード.
//std::vector<std::shared_ptr<CPlayer>> CPlayerManager::GetControlPlayer(int index)
//{
//	if (index >= 0 && index < (int)m_pPlayers.size()) {
//		return m_pPlayers[index].get();
//	}
//	return nullptr;
//}

void CPlayerManager::SwitchActivePlayer()
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

D3DXVECTOR3 CPlayerManager::GetPosition(int index) const
{
	if (index >= 0 && index < static_cast<int>(m_pPlayers.size()))
	{
		return m_pPlayers[index]->GetPosition();
	}
	return D3DXVECTOR3(0, 0, 0);
}

D3DXVECTOR3 CPlayerManager::GetRotation(int idx) const
{
	if (idx >= 0 && idx < static_cast<int>(m_pPlayers.size()))
	{
		return m_pPlayers[idx]->GetRotation();
	}
	return D3DXVECTOR3(0, 0, 0);
}

void CPlayerManager::SetShotManager(std::shared_ptr<CShotManager>& mgr)
{
	m_ShotManager = mgr;

	//すでにいる前COMに渡す　
	for (auto& up : m_pPlayers) {
		if (auto* com = dynamic_cast<CComPlayer*>(up.get())) {	//CComPlayerなら生のポインタにして渡す.所有権は渡さない
			com->AttachShotManager(m_ShotManager);	//weak_ptrに渡す
		}
	}

}

void CPlayerManager::SetBodyAndCannon(std::shared_ptr<CBody> body, std::shared_ptr<CCannon> cannon)
{
	//このクラス内で使えるようにする.
	m_pBody = body;
	m_pCannon = cannon;

	for (auto player : m_pPlayers)
	{
		player->SetCBody(body);
		player->SetCCannon(cannon);
	}
}

//COMとプレイヤーのコントローラー処理
void CPlayerManager::SyncByPadConnection()
{
#if 1
		//パッド接続状態の更新と、切断処理
		for (int pid = 0; pid < (int)m_Pads.size(); ++pid)
		{
			CXInput* pad = m_Pads[pid].get();	//生のポインタに変換して入れる
			pad->Update();

			const bool now = pad->IsConnect();
			const bool prev = m_PadConnected[pid];

			//未接続処理
			if (now && !prev)
			{
				//未割当なら誰かに必ず渡す
				if (m_PadIndex[pid] < 0)
				{
					int idx = FindFirstComPlayer(); //COM稼働中のプレイヤー
					if (idx < 0) {
						//COMが居ない＝今はプレイヤー0人or誰かがプレイヤー
						//今のプレイヤー（HasControl==true）に渡す
						idx = -1;
						for (int i = 0; i < (int)m_pPlayers.size(); ++i)
						{
							if (m_pPlayers[i]->HasControl())
							{ 
								idx = i;
								break;
							}
							if (idx < 0 && !m_pPlayers.empty())
							{
								idx = 0;
							}
						}
					}

					if (idx >= 0) {
						//そのプレイヤーに別padが付いてたら解除
						if (m_PlayerPad[idx] >= 0) {
							int old = m_PlayerPad[idx];
							m_PadIndex[old] = -1;
						}
						m_PadIndex[pid] = idx;   //pad -> player
						m_PlayerPad[idx] = pid;  //player -> pad
					}
				}
			}
			//切断処理
			else if (!now && prev)
			{
				const int owner = m_PadIndex[pid];
				if (owner >= 0)
				{
					//パッド参照解除
					m_pPlayers[owner]->SetPadRef(nullptr);
					m_PlayerPad[owner] = -1;
					m_PadIndex[pid] = -1;

					m_PlayerPad[owner] = FindFirstComPlayer();	//PAD参照切ったあとCOM入れる
				}
			}
			m_PadConnected[pid] = now;
		}

		//全プレイヤーの状態をpad割当と接続に基づいて再同期
		for (int idx = 0; idx < (int)m_pPlayers.size(); ++idx)
		{
			const int pid = (idx < (int)m_PlayerPad.size() ? m_PlayerPad[idx] : -1);
			const bool human = (pid >= 0) && m_PadConnected[pid];

			m_pPlayers[idx]->SetHasControl(human);
			m_pPlayers[idx]->SetPadRef(human ? m_Pads[pid].get() : nullptr);

			if (auto com = std::dynamic_pointer_cast<CComPlayer>(m_pPlayers[idx])) {
				com->SetComEnabled(!human); //プレイヤーならCOM停止/未接続ならCOM稼働
		}
	}
#endif
}

int CPlayerManager::FindFirstComPlayer() const
{
	for (int i = 0; i < (int)m_pPlayers.size(); ++i)
	{
		if (auto com = std::dynamic_pointer_cast<CComPlayer>(m_pPlayers[i]))
		{
			//com有効、操作権は無効
			if (com->IsComEnabled() && !m_pPlayers[i]->HasControl())
			{
				return i;
			}
		}
		return -1;	//未割り当て
	}
}

#if 1
void CPlayerManager::SetPlayerTuningAll(const TankTuning& t) {
	for (auto& p : m_pPlayers) p->SetTuning(t);
}
void CPlayerManager::SetPlayerTuning(int idx, const TankTuning& t) {
	if (idx >= 0 && idx < (int)m_pPlayers.size()) m_pPlayers[idx]->SetTuning(t);
}
#endif


