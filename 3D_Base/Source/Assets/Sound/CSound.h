#pragma once
//-----ライブラリ-----
#include <vector>
#include <map>

// XAudio2関連
#include <xaudio2.h>
#pragma comment(lib, "XAudio2.lib")

//-----外部クラス-----
#include "Assets//Sound//FileReader//FileType//CFileType.h" // WaveDataを格納

//=============================================================
//	サウンドクラス
//		midi,mp3,wav形式ファイルの再生・停止等を行う
//=============================================================
class CSound
{
public:
	CSound(IXAudio2* pXAudio2, const WaveData& wave);
	CSound(IXAudio2* pXAudio2, const Mp3Data& mp3);
	~CSound();

	//=====ソースボイス破棄======
	void CleanUpFinishVoices();
	//=========================

	//========効果音再生=========
	bool PlaySE();
	//==========================

	//========ループ再生=========
	bool PlayLoop();
	//==========================

	//========再生停止===========
	void Stop();
	//==========================

private:
	IXAudio2*	m_pXAudio2;
	WaveData	m_Wave;
	Mp3Data		m_Mp3;
	SoundType	m_Type;
	std::vector<IXAudio2SourceVoice*> m_pVoices; // 実行中のソースボイス

	bool m_IsPlaying = false; // 再生中かどうか
};

//命名規則とハンガリアン記法.
//m_:member（メンバー）.
//g_:global（グローバル）.
//s_:static（スタティック）.
//iNo:int（int型の番号）.
//str:string（ストリング：文字列）.