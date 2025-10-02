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

#if 0
	for (int i = 0; i < (int)m_Pads.size(); i++)
	{
		CXInput* pad = m_Pads[i].get();
		pad->Update();
		const bool now = pad->IsConnect();
		const bool prev = m_PadConnected[i];

		if (now && !prev)
		{
			if (m_PadIndex[i] < 0)
			{
				const int index = FindFirstComPlayer();	
			}
		}
	}
#endif
#if 0

			//未割り当てであればCOMの誰かに割り当てる
			if (now && !prev)
			{
				if (m_PadIndex[i] < 0)
				{
					const int index = FindFirstComPlayer();	//comを返す

					if (index >= 0)
					{
						//その前に割り当てていれば解除
						if (m_PlayerPad[index] >= 0)
						{
							int old = m_PlayerPad[index];
							m_PadIndex[old] = -1;	//未割り当て
						}
						m_PadIndex[i] = index;
						m_PlayerPad[index] = i;

						//comは停止、プレイヤーも有効にして操作権をあげるPadも接続する
						if (auto com = std::dynamic_pointer_cast<CComPlayer>(m_pPlayers[index]))
						{
							com->SetComEnabled(false);	//COM無効
							m_pPlayers[index]->SetHasControl(true);	//プレイヤーの操作権が有効
							m_pPlayers[index]->SetPadRef(pad);	//パッドにプレイヤー割り当て
						}
					}
				}
				//切断された場合はCOMに戻す
				if (!now && prev)
				{
					const int owner = m_PadIndex[i];
					if (owner >= 0)
					{
						m_pPlayers[owner]->SetHasControl(false);	//操作権無効
						if (auto com = std::dynamic_pointer_cast<CComPlayer>(m_pPlayers[owner]))
						{
							com->SetComEnabled(true);				//COM有効
							m_pPlayers[owner]->SetPadRef(nullptr);	//Pad設定無効

							//未割り当てに設定
							m_PadIndex[i] = -1;
							m_PlayerPad[owner] = -1;
						}
					}
					m_PadConnected[i] = now;	//切断された状態
				}
				//人とCOMを認識する
				for (int i = 0; i < (int)m_pPlayers.size(); i++)
				{
					const int pid = (i < (int)m_PlayerPad.size() ? m_PlayerPad[i] : -1);
					const bool player = (pid >= 0) && m_PadConnected[pid];

					m_pPlayers[i]->SetHasControl(player);
					//m_pPlayers[i]->SetPadRef(player ? m_Pads[pid].get() : nullptr);	
#if 1
					if (player)
					{
						m_pPlayers[i]->SetPadRef(m_Pads[pid].get());
					}
					else
					{
						m_pPlayers[i]->SetPadRef(nullptr);
					}
#endif
					if (auto com = std::dynamic_pointer_cast<CComPlayer>(m_pPlayers[i]))
					{
						com->SetComEnabled(!player);	//プレイヤーならCOM停止、そうでなければCOM稼働
					}
				}
			}
		}

#if 0	

		//毎フレーム、割当整合で人/COMを決定
		for (int i = 0; i < (int)m_pPlayers.size(); ++i) {
			const int pid = (i < (int)m_playerPad.size() ? m_playerPad[i] : -1);
			const bool human = (pid >= 0) && m_padConnected[pid];

			m_pPlayers[i]->SetHasControl(human);
			m_pPlayers[i]->SetPadRef(human ? m_pads[pid].get() : nullptr);

			if (auto com = std::dynamic_pointer_cast<CComPlayer>(m_pPlayers[i])) {
				com->SetAIEnabled(!human); // 人間ならAI停止, そうでなければAI稼働
			}
		}
	}
#endif
}

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

	////操作プレイヤーを更新
	//if (m_ActivePlayerIndex >= 0 && m_ActivePlayerIndex < count) {
	//	m_pPlayers[m_ActivePlayerIndex]->Update();
	//}

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

	SyncByPadConnection();

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


void CPlayerManager::SyncByPadConnection()
{

	for (int i = 0; i < (int)m_Pads.size(); ++i)
	{
		CXInput* pad = m_Pads[i].get();	
		pad->Update();	
		const bool now = pad->IsConnect();		//接続はされていない
		const bool prev = m_PadConnected[i];	//前のフレームでの接続状態

		//未割り当てであればCOMの誰かに割り当てる
		if (now && !prev)
		{
			if (m_PadIndex[i] < 0)
			{
				const int index = FindFirstComPlayer();	//comを返す
				
				if (index >= 0)
				{
					//その前に割り当てていれば解除
					if (m_PlayerPad[index] >= 0)
					{
						int old = m_PlayerPad[index];
						m_PadIndex[old] = -1;	//未割り当て
					}
					m_PadIndex[i] = index;
					m_PlayerPad[index] = i;

					//comは停止、プレイヤーも有効にして操作権をあげるPadも接続する
					if (auto com = std::dynamic_pointer_cast<CComPlayer>(m_pPlayers[index]))
					{
						com->SetComEnabled(false);	//COM無効
						m_pPlayers[index]->SetHasControl(true);	//プレイヤーの操作権が有効
						m_pPlayers[index]->SetPadRef(pad);	//パッドにプレイヤー割り当て
					}
				}
			}
			//切断された場合はCOMに戻す
			if (!now && prev)
			{
				const int owner = m_PadIndex[i];
				if (owner >= 0)
				{
					m_pPlayers[owner]->SetHasControl(false);	//操作権無効
					if (auto com = std::dynamic_pointer_cast<CComPlayer>(m_pPlayers[owner]))
					{
						com->SetComEnabled(true);				//COM有効
						m_pPlayers[owner]->SetPadRef(nullptr);	//Pad設定無効

						//未割り当てに設定
						m_PadIndex[i] = -1;
						m_PlayerPad[owner] = -1;
					}
				}
				m_PadConnected[i] = now;	//切断された状態
			}
			//人とCOMを認識する
			for (int i = 0; i < (int)m_pPlayers.size(); i++)
			{	

				const int pid = (i < (int)m_PlayerPad.size() ? m_PlayerPad[i] : -1);
				const bool player = (pid >= 0) && m_PadConnected[pid];


				m_pPlayers[i]->SetHasControl(player);	
				//m_pPlayers[i]->SetPadRef(player ? m_Pads[pid].get() : nullptr);	
#if 1

				if (player)
				{
					m_pPlayers[i]->SetPadRef(m_Pads[pid].get());
				}
				else
				{
					m_pPlayers[i]->SetPadRef(nullptr);
				}
#endif
				if (auto com = std::dynamic_pointer_cast<CComPlayer>(m_pPlayers[i]))
				{
					com->SetComEnabled(!player);	//プレイヤーならCOM停止、そうでなければCOM稼働
				}
			}
		}
	}

#if 0	

		//毎フレーム、割当整合で人/COMを決定
		for (int i = 0; i < (int)m_pPlayers.size(); ++i) {
			const int pid = (i < (int)m_playerPad.size() ? m_playerPad[i] : -1);
			const bool human = (pid >= 0) && m_padConnected[pid];

			m_pPlayers[i]->SetHasControl(human);
			m_pPlayers[i]->SetPadRef(human ? m_pads[pid].get() : nullptr);

			if (auto com = std::dynamic_pointer_cast<CComPlayer>(m_pPlayers[i])) {
				com->SetAIEnabled(!human); // 人間ならAI停止, そうでなければAI稼働
			}
		}
	}
#endif
}

int CPlayerManager::FindFirstComPlayer() const
{
#if 1
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
		return -1;	//無効値
	}
#endif
}


