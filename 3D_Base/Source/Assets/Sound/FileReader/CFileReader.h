#pragma once
#include "Assets//Sound///FileReader//FileType//CFileType.h" // ファイルタイプ

//=============================================================
// Soundファイルロードクラス
//=============================================================
class CFileReader
{
public:
	CFileReader();
	~CFileReader();

	
	//=====Waveファイル読み込み=====
	bool LoadWaveFile(const std::wstring& wFilePath, WaveData& outData);
	//============================

	//=====Mp3ファイル読み込み=====
	bool LoadMp3File(const std::wstring& wFilePath, Mp3Data& outData);
	//============================
};
