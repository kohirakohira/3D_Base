#include "CSound.h"
#include <stdexcept> // std::runtime_error

CSound::CSound(IXAudio2* pXAudio2, const WaveData& wave)
	: m_pXAudio2(pXAudio2)
	, m_Wave(std::move(const_cast<WaveData&>(wave)))
	, m_Type(SoundType::Wave)
{
}

CSound::CSound(IXAudio2* pXAudio2, const Mp3Data& mp3)
	: m_pXAudio2(pXAudio2)
	, m_Mp3(std::move(const_cast<Mp3Data&>(mp3)))
	, m_Type(SoundType::Mp3)
{
}

CSound::~CSound()
{
	for (auto* voice : m_pVoices)
	{
		voice->DestroyVoice();
	}
}

//=====ソースボイス破棄======
void CSound::CleanUpFinishVoices()
{
	// 再生が終了したソースボイスを破棄
	m_pVoices.erase(
		std::remove_if(
			m_pVoices.begin(),
			m_pVoices.end(),
			[](IXAudio2SourceVoice* voice)
			{
				if (voice)
				{
					XAUDIO2_VOICE_STATE state;
					voice->GetState(&state);
					if (state.BuffersQueued == 0)
					{
						voice->DestroyVoice();
						return true; // 削除対象
					}
				}
				return false; // 削除しない
			}
		),
		m_pVoices.end()
	);
}
//=========================

//========効果音再生=========
bool CSound::PlaySE()
{
	CleanUpFinishVoices();

	const WAVEFORMATEX* format = nullptr;
	const BYTE* buffer = nullptr;
	DWORD bufferSize = 0;

	if (m_Type == SoundType::Wave)
	{
		format = &m_Wave.m_WavFormat;
		buffer = (BYTE*)m_Wave.m_SoundBuffer;
		bufferSize = m_Wave.m_Size;
	}
	else // MP3
	{
		format = &m_Mp3.m_Mp3Format;
		buffer = (BYTE*)m_Mp3.m_SoundBuffer;
		bufferSize = m_Mp3.m_Size;
	}

	IXAudio2SourceVoice* voice;
	if (FAILED(m_pXAudio2->CreateSourceVoice(&voice, format)))
	{
		return false;
	}

	m_pVoices.push_back(voice);

	XAUDIO2_BUFFER buf{};
	buf.pAudioData = buffer;
	buf.AudioBytes = bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	voice->SubmitSourceBuffer(&buf);
	voice->Start();

	return true;
}
//==========================

//========ループ再生=========
bool CSound::PlayLoop()
{
	CleanUpFinishVoices();

	const WAVEFORMATEX* format = nullptr;
	const BYTE* buffer = nullptr;
	DWORD bufferSize = 0;

	if (m_Type == SoundType::Wave)
	{
		format = &m_Wave.m_WavFormat;
		buffer = (BYTE*)m_Wave.m_SoundBuffer;
		bufferSize = m_Wave.m_Size;
	}
	else // MP3
	{
		format = &m_Mp3.m_Mp3Format;
		buffer = (BYTE*)m_Mp3.m_SoundBuffer;
		bufferSize = m_Mp3.m_Size;
	}

	IXAudio2SourceVoice* voice;
	if (FAILED(m_pXAudio2->CreateSourceVoice(&voice, format)))
	{
		return false;
	}

	m_pVoices.push_back(voice);

	XAUDIO2_BUFFER buf{};
	buf.pAudioData = buffer;
	buf.AudioBytes = bufferSize;
	buf.LoopCount = XAUDIO2_LOOP_INFINITE;

	voice->SubmitSourceBuffer(&buf);
	voice->Start();

	return true;
}
//==========================

//========再生停止===========
void CSound::Stop()
{
	for (auto* voice : m_pVoices)
	{
		voice->Stop(0);
	}
}
//==========================
