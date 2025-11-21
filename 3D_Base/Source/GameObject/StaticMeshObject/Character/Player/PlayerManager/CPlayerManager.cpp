#include "CPlayerManager.h"

#undef max;
#undef min;

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

	m_pPlayers.clear();
	m_pPlayers.reserve(PLAYER_MAX);


	for (int i = 0; i < PLAYER_MAX; ++i) {
		auto com = std::make_shared<CComPlayer>();
		//auto player = std::make_shared<CPlayer>();
		com->Initialize(i);
		//player->Initialize(i);

		//既定はCOM
		com->SetComEnabled(true);
		com->SetHasControl(false);

		// 最初の1体だけ人間操作にする
		if (i == 0) {
			com->SetComEnabled(false); //COM停止,人間化
			com->SetHasControl(true);  //入力を読むのはこの個体だけ
			com->SetKeyBoadEnble(true);
			com->SetPadRef(nullptr);
		}
		else
		{
			com->SetKeyBoadEnble(false);
			com->SetPadRef(nullptr);
		}
		m_pPlayers.push_back(std::move(com));
	}

	m_PadConnected.fill(false);		//最初の接続状態は無効
	m_PadIndex.fill(-1);			//最初は無効値で埋める
	m_PlayerPad.assign(m_pPlayers.size(), -1);	//サイズ取得と中身を同じ要素で埋める

	//サイズ分回してCXInputを生成する
	for (DWORD id = 0; id < m_Pads.size(); ++id)	//XInput側がDWORDなのであわせておく
	{
		m_Pads[id] = std::make_unique<CXInput>(id);
	}

	SyncByPadConnection();

	for (auto& player : m_pPlayers) {
		if (auto com = std::dynamic_pointer_cast<CComPlayer>(player)) {
			com->SetPlayersRef(&m_pPlayers);
		}
	}
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

void CPlayerManager::SetPlayerScale(int index, const float& xyz)
{
	if (index < m_pPlayers.size())
	{
		m_pPlayers[index]->SetTankScale(xyz);
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

	//コントローラー判定
	SyncByPadConnection();

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
					//ターゲットがプレイヤー限定なのであとで消す
					//com->SetTarget(target);
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

std::shared_ptr<CPlayer> CPlayerManager::GetControlPlayer(int index)
{
	if (index >= 0 && index < (int)m_pPlayers.size()) {
		return m_pPlayers[index];
	}
	return nullptr;
}

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

void CPlayerManager::SetPlayerTuningAll(const TankTuning& t)
{
	for (auto& player : m_pPlayers)player->SetTuning(t);
}

void CPlayerManager::SetPlayerTuning(int idx, const TankTuning& t)
{
	if (idx >= 0 && idx < (int)m_pPlayers.size())m_pPlayers[idx]->SetTune(t);
}

void CPlayerManager::SetComObstacles(const std::vector<CComPlayer::SimpleObstacle>* obstacles)
{
	for (auto& up : m_pPlayers)
	{
		if (auto* com = dynamic_cast<CComPlayer*>(up.get()))
		{
			com->SetSimpleObstacles(obstacles);
		}
	}
}

void CPlayerManager::SyncByPadConnection()
{
	//接続されているかどうかを取得する
	for (int padIndex = 0; padIndex < (int)m_Pads.size(); ++padIndex) {
		CXInput* pad = m_Pads[padIndex].get();	
		pad->Update();

		//今のフレームと前のフレームの状態
		const bool now = pad->IsConnect();			
		const bool prev = m_PadConnected[padIndex];

		if (now != prev)
		{
			if (now)
			{
				//接続
				int targetIndex;
				if (padIndex < static_cast<int>(m_pPlayers.size()))
				{
					targetIndex = padIndex;
				}
				else
				{
					targetIndex = -1;	//割り当てなし
				}

				if (targetIndex >= 0)
				{
					//paleyrにすでにpadがついていたら解除
					if (m_PlayerPad[targetIndex] >= 0)
					{
						int oldpadIndex = m_PlayerPad[targetIndex];
						m_PadIndex[oldpadIndex] = -1;	//古いやつは割り当てなし
					}
					//新しい双方向対応を設定
					m_PadIndex[padIndex] = targetIndex;		//pad->player
					m_PlayerPad[targetIndex] = padIndex;	//plaeyr->pad

				}
			}
			else
			{
				//切断
				const int owner = m_PadIndex[padIndex];
				if (owner >= 0)
				{
					m_PlayerPad[owner] = -1;
					m_PadIndex[padIndex] = -1;
				}
			}
			//今のフレームのものを前回のものとして記録する
			m_PadConnected[padIndex] = now;
		}
	}

	for (int index = 0; index < static_cast<int>(m_pPlayers.size()); index++)
	{
		//player->padの逆引き.範囲外なら-1
		int padIndex;
		if (index < static_cast<int>(m_PlayerPad.size()))
		{
			padIndex = m_PlayerPad[index];
		}
		else
		{
			padIndex = -1;
		}

		//padが有効か判定
		bool padOn = false;
		if (padIndex >= 0)
		{
			if (m_PadConnected[padIndex])
			{
				padOn = true;
			}
		}

		const bool isP1 = (index == m_keyboardPlayer);

		bool player = false;

		//1Pとそれ以外
		if (isP1)
		{
			//1Pは常にキーボードがonでpadも併用できる
			player = true;
			m_pPlayers[index]->SetKeyBoadEnble(true);

			//padOnが真のときだけpadsを参照
			CXInput* padRef = nullptr;
			if (padOn)
			{
				padRef = m_Pads[padIndex].get();
			}
			m_pPlayers[index]->SetPadRef(padRef);
		}
		else
		{
			//2Pから4Pはpadがあればplayer
			player = padOn;
			m_pPlayers[index]->SetKeyBoadEnble(false);

			CXInput* padRef = nullptr;
			if (padOn)
			{
				padRef = m_Pads[padIndex].get();
			}
			m_pPlayers[index]->SetPadRef(padRef);
		}

		//操作権の反映
		m_pPlayers[index]->SetHasControl(player);

		//comの有無,playerならcomはoff
		if (auto com = std::dynamic_pointer_cast<CComPlayer>(m_pPlayers[index]))
		{
			com->SetComEnabled(!player);
		}
	}
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

int CPlayerManager::FindFirstPadReceiver(int StartIndex) const
{
	for (int i = std::max(0, StartIndex); i < (int)m_pPlayers.size(); ++i)
	{
		if (i < (int)m_PlayerPad.size() && m_PlayerPad[i] < 0) return i;
	}
	return -1;
}



