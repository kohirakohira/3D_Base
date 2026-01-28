#pragma once

class CComSoundController
{
public:
	CComSoundController();

	//どのCOMかをセット
	void SetPlayerID(int id) { m_PlayerID = id; }

	//移動サウンドの更新
	void UpdateMoveSound(bool isMoving);

private:
	int m_PlayerID;
	bool m_WasMoving;	
};