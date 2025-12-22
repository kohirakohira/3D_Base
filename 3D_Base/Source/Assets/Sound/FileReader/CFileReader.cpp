#include "CFileReader.h"

//-----ライブラリ-----
// １からライブラリなしで作成するとかなりの時間がかかるため、
// Media Foundationを使用してmp3を再生する
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h> // IMFSourceReader
#include <vector>

#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

CFileReader::CFileReader()
{
}

CFileReader::~CFileReader()
{
}

bool CFileReader::LoadWaveFile(const std::wstring& wFilePath, WaveData& outData)
{
	// 有効なメモリが存在しているか確認
	if (outData.m_SoundBuffer)
	{
		// m_SoundBufferを解放
		delete[] outData.m_SoundBuffer;

		// エラーが起こらないようにnullptr
		outData.m_SoundBuffer = nullptr;
	}

	HMMIO mmioHandle = nullptr;	// MMIOハンドル
	MMCKINFO chunkInfo{};		// チャンクの情報
	MMCKINFO riffChunkInfo{};	// RIFFチャンクの情報

	// WAVファイルを開く
	mmioHandle = mmioOpen(
		(LPWSTR)wFilePath.data(), // ファイル名
		nullptr,				  // MMIO情報
		MMIO_READ				  // オープンモード
	);

	if (mmioHandle == nullptr)
	{
		// オープン失敗
		return false;
	}

	// RIFFチャンクに進入するためにfccTypeにWAVEを設定する
	riffChunkInfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');

	// RIFFチャンクに進入する
	if (mmioDescend(
		mmioHandle,		// MMIOハンドル
		&riffChunkInfo,	// 取得したRIFFチャンクの情報
		nullptr,		// 親チャンク
		MMIO_FINDRIFF	// 取得情報の種類
	) != MMSYSERR_NOERROR)
	{
		// 失敗
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return false;
	}

	// 進入先のチャンクを'fmt'として設定する
	chunkInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(
		mmioHandle,		// MMIOハンドル
		&chunkInfo,		// 取得したチャンクの情報
		&riffChunkInfo,	// 親チャンク
		MMIO_FINDCHUNK	// 取得情報の種類
	) != MMSYSERR_NOERROR)
	{
		// fmtチャンクがない
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return false;
	}

	// fmtデータの読み込み
	DWORD readSize = mmioRead(
		mmioHandle,						//　MMIOハンドル
		(HPSTR)&outData.m_WavFormat,	//　読み込み用バッファ
		chunkInfo.cksize);				//　バッファサイズ

	// サイズチェック
	if (readSize != chunkInfo.cksize)
	{
		// 読み込みサイズが一致していないのでエラー
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return false;
	}

	// フォーマットチェック
	if (outData.m_WavFormat.wFormatTag != WAVE_FORMAT_PCM)
	{
		// Waveフォーマットエラーです
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return false;
	}

	// fmtチャンクを退出する
	if (mmioAscend(mmioHandle, &chunkInfo, 0) != MMSYSERR_NOERROR)
	{
		// fmtチャンク退出失敗
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return false;
	}

	// dataチャンクに進入する
	chunkInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(
		mmioHandle,		// MMIOハンドル
		&chunkInfo,		// 取得したチャンクの情報
		&riffChunkInfo,	// 親チャンク
		MMIO_FINDCHUNK	// 取得情報の種類
	) != MMSYSERR_NOERROR)
	{
		// 進入失敗
		mmioClose(mmioHandle, MMIO_FHOPEN);
		return false;
	}

	// 音データの読み込み
	outData.m_Size = chunkInfo.cksize;
	outData.m_SoundBuffer = new char[chunkInfo.cksize];
	readSize = mmioRead(mmioHandle, (HPSTR)outData.m_SoundBuffer, chunkInfo.cksize);
	if (readSize != chunkInfo.cksize)
	{
		// dataチャンク読み込み失敗
		mmioClose(mmioHandle, MMIO_FHOPEN);
		delete[] outData.m_SoundBuffer;
		outData.m_SoundBuffer = nullptr;
		return false;
	}

	// ファイルを閉じる
	mmioClose(mmioHandle, MMIO_FHOPEN);

	return true;
}

bool CFileReader::LoadMp3File(const std::wstring& wFilePath, Mp3Data& outData)
{
	// 既存メモリの解放（Wave と統一）
	if (outData.m_SoundBuffer)
	{
		delete[] outData.m_SoundBuffer;
		outData.m_SoundBuffer = nullptr;
		outData.m_Size = 0;
	}

	outData.path = wFilePath;

	IMFSourceReader* pReader = nullptr;
	IMFMediaType* pPcmType = nullptr;
	IMFMediaType* pOutType = nullptr;

	HRESULT hr = S_OK;

	//---- SourceReader 生成 ----
	hr = MFCreateSourceReaderFromURL(wFilePath.c_str(), nullptr, &pReader);
	if (FAILED(hr)) return false;

	//---- 出力を PCM に強制 ----
	MFCreateMediaType(&pPcmType);
	pPcmType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pPcmType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);

	pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pPcmType);
	pPcmType->Release();

	//---- PCM の WAVEFORMATEX を取得 ----
	hr = pReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutType);
	if (FAILED(hr))
	{
		pReader->Release();
		return false;
	}

	WAVEFORMATEX* wfx = nullptr;
	UINT32 wfxSize = 0;

	hr = MFCreateWaveFormatExFromMFMediaType(pOutType, &wfx, &wfxSize);
	pOutType->Release();

	if (FAILED(hr))
	{
		pReader->Release();
		return false;
	}

	// WAVEFORMATEX コピー
	memcpy(&outData.m_Mp3Format, wfx, sizeof(WAVEFORMATEX));
	CoTaskMemFree(wfx);

	//---- デコードされた PCM データを収集 ----
	std::vector<BYTE> pcmData;

	while (true)
	{
		DWORD flags = 0;
		IMFSample* pSample = nullptr;

		hr = pReader->ReadSample(
			MF_SOURCE_READER_FIRST_AUDIO_STREAM,
			0, nullptr, &flags, nullptr, &pSample);

		if (FAILED(hr) || (flags & MF_SOURCE_READERF_ENDOFSTREAM))
			break;

		if (pSample)
		{
			IMFMediaBuffer* pBuffer = nullptr;
			pSample->ConvertToContiguousBuffer(&pBuffer);

			BYTE* pBytes = nullptr;
			DWORD maxLen = 0, curLen = 0;

			pBuffer->Lock(&pBytes, &maxLen, &curLen);

			size_t oldSize = pcmData.size();
			pcmData.resize(oldSize + curLen);
			memcpy(pcmData.data() + oldSize, pBytes, curLen);

			pBuffer->Unlock();
			pBuffer->Release();
			pSample->Release();
		}
	}

	pReader->Release();

	//---- 読み込んだ PCM を Mp3Data へコピー ----
	outData.m_Size = static_cast<DWORD>(pcmData.size());
	outData.m_SoundBuffer = new char[outData.m_Size];
	memcpy(outData.m_SoundBuffer, pcmData.data(), outData.m_Size);

	return true;
}
