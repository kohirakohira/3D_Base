#pragma once
//=============================================================
// Soundファイルの構造体を定義
//=============================================================

//=====Wave構造体=====
struct WaveData
{
    WAVEFORMATEX	m_WavFormat;	// Waveファイルフォーマットに関する情報
    char* m_SoundBuffer;	// 音の波形データ
    DWORD			m_Size;			// サイズ

    WaveData()
        : m_SoundBuffer(nullptr)
        , m_Size(0)
    {
        ZeroMemory(&m_WavFormat, sizeof(m_WavFormat));
    }

    // new[] で確保したメモリを delete[] で確実に解放
    ~WaveData()
    {
        delete[] m_SoundBuffer;
    }

    // コピーを禁止
    WaveData(const WaveData&) = delete;
    WaveData& operator = (const WaveData&) = delete;

    // ムーブコンスタントラクタと代入演算子
    WaveData(WaveData&& other) noexcept
        /* noexcept : 関数宣言の最後に追加すると
                      例外を投げないことをコンパイラに伝える*/
        : m_WavFormat(other.m_WavFormat)
        , m_SoundBuffer(other.m_SoundBuffer)
        , m_Size(other.m_Size)
    {
        other.m_SoundBuffer = nullptr;	// 所有権を奪ったので無効化
        other.m_Size = 0;
    }

    WaveData& operator = (WaveData&& other) noexcept
    {
        if (this != &other)
        {
            delete[] m_SoundBuffer;	// 既存のメモリを解放
            // 他のオブジェクトから所有権を奪う
            m_WavFormat = other.m_WavFormat;
            m_SoundBuffer = other.m_SoundBuffer;
            m_Size = other.m_Size;

            // 移動元は無効化
            other.m_SoundBuffer = nullptr;
            other.m_Size = 0;
        }
        return *this;
    }
};
//===================

//=====Mp3構造体=====
struct Mp3Data
{
    WAVEFORMATEX	m_Mp3Format;   // PCMフォーマット
    char*           m_SoundBuffer; // 音の波形データ
    DWORD			m_Size;        // PCMサイズ
    std::wstring	path;          // MP3ファイルのパス

    Mp3Data()
        : m_SoundBuffer(nullptr)
        , m_Size(0)
    {
        ZeroMemory(&m_Mp3Format, sizeof(m_Mp3Format));
    }

    // new[] で確保したメモリを delete[] で確実に解放
    ~Mp3Data()
    {
        delete[] m_SoundBuffer;
    }

    // コピー禁止
    Mp3Data(const Mp3Data&) = delete;
    Mp3Data& operator = (const Mp3Data&) = delete;

    // ムーブコンストラクタと代入演算子
    Mp3Data(Mp3Data&& other) noexcept
        : m_Mp3Format   (other.m_Mp3Format)
        , m_SoundBuffer (other.m_SoundBuffer)
        , m_Size        (other.m_Size)
        , path          (std::move(other.path))
    {
        other.m_SoundBuffer = nullptr;	// 所有権を奪ったので無効化
        other.m_Size        = 0;
    }

    // ムーブ代入演算子
    Mp3Data& operator = (Mp3Data&& other) noexcept
    {
        if (this != &other)
        {
            delete[] m_SoundBuffer;	// 既存のメモリを解放
            // 他のオブジェクトから所有権を奪う
            m_Mp3Format   = other.m_Mp3Format;
            m_SoundBuffer = other.m_SoundBuffer;
            m_Size        = other.m_Size;
            path          = std::move(other.path);

            // 移動元は無効化
            other.m_SoundBuffer = nullptr;
            other.m_Size        = 0;
        }
        return *this;
    }
};
//==================

//=====サウンドタイプ=====
enum class SoundType
{
    Wave,
    Mp3
};
//======================