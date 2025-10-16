#pragma once
#include <iostream>

#include "CCharacter.h"

#include <vector>
#include "CPlayer.h"
#include "CShot.h"
#include "Global.h"
#include "CComPlayer.h"

//↓松岡.
#include "CBody.h"
#include "CCannon.h"

class CPlayerManager
	: public CCharacter
{
public:
	CPlayerManager();
	~CPlayerManager();

	void Initialize();
	void AttachMeshesToPlayer(int index, std::shared_ptr<CStaticMesh> body, std::shared_ptr<CStaticMesh> cannon);
	void SetPlayerPosition(int index, const D3DXVECTOR3& pos);
	//↓松岡.
	void SetPlayerRotation(int index, const D3DXVECTOR3& rad);
	void Update() override;
	void Draw (D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;
	
	D3DXVECTOR3 GetPosition();
	std::shared_ptr<CPlayer> GetControlPlayer(int index);

	void SwitchActivePlayer();

	//プレイヤーの位置と回転を取得.引数には各プレイヤーを入れる
	D3DXVECTOR3 GetPosition(int index)const;
	D3DXVECTOR3 GetRotation(int index)const;

	//追尾のデバック
	//ロック対象のインデックス指定
	void SetLockTargetIndex(int index) { m_LockTargetIndex = index; }



	//↓松岡
	//子オブジェクトに各BodyとCannonを設定してあげる関数.
	void SetBodyAndCannon(std::shared_ptr<CBody> body, std::shared_ptr<CCannon> cannon);





private:
	//↓松岡.
	std::shared_ptr<CBody>					m_pBody;
	std::shared_ptr<CCannon>				m_pCannon;



    std::vector<std::shared_ptr<CPlayer>>	m_pPlayers;
    std::vector<std::unique_ptr<CShot>>		m_pShots;
	int m_ActivePlayerIndex;	// 現在操作中のプレイヤー(デバッグ用)

	int m_LockTargetIndex = -1;	//ロック無効(デバック用)
};


