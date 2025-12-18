//-----ライブラリ-----
#include <mfapi.h> // MediaFoundatiom

#include "CSoundManager.h" 

CSoundManager::CSoundManager()
    : m_pXAudio2        (nullptr)
    , m_pMasteringVoice (nullptr)
{
}

CSoundManager::~CSoundManager()
{
    Release();
}

bool CSoundManager::Load()
{
    // COM 初期化
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        return false;
    }

    // Media Foundation初期化
    hr = MFStartup(MF_VERSION);
    if (FAILED(hr))
    {
        CoUninitialize();
        return false;
    }

    // XAudio2 初期化
    hr = XAudio2Create(&m_pXAudio2);
    if (FAILED(hr))
    {
        MFShutdown();
        CoUninitialize();
        return false;
    }

    // マスターボイス作成
    hr = m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice);
    if (FAILED(hr))
    {
        m_pXAudio2->Release();
        MFShutdown();
        CoUninitialize();
        return false;
    }

    // WAV読み込み
    CFileReader reader;

    // 列挙型にファイルパスを読み込む
    std::map<SE, std::wstring> filePaths =
    {
        {SE::SE_QUACK,      L"Data\\Sound\\SE\\quack_5.wav"},
        {SE::SE_CLEAR,      L"Data\\Sound\\SE\\Clear.wav"},
        {SE::BGM_sample,    L"Data\\Sound\\BGM\\Digi_Rock_04.mp3"},
    };

    for (auto& path : filePaths)
    {
        const std::wstring& file = path.second;

        // 拡張子取得（※大文字小文字対応）
        std::wstring ext;
        size_t pos = file.find_last_of(L'.');
        if (pos != std::wstring::npos)
        {
            ext = file.substr(pos + 1);
            for (auto& c : ext) c = towlower(c);
        }

        // WAV の場合
        if (ext == L"wav")
        {
            WaveData wave;
            if (!reader.LoadWaveFile(file, wave))
            {
                return false;
            }
            m_SoundMap[path.first] = new CSound(m_pXAudio2, wave);
        }
        // MP3 の場合
        else if (ext == L"mp3")
        {
            Mp3Data mp3;
            if (!reader.LoadMp3File(file, mp3))
            {
                return false;
            }
            m_SoundMap[path.first] = new CSound(m_pXAudio2, mp3);
        }
        else
        {
            // 未対応フォーマット
            return false;
        }
    }

    return true;
}

void CSoundManager::Release()
{
    for (auto& map : m_SoundMap)
    {
        delete map.second;
    }
    m_SoundMap.clear();

    if (m_pMasteringVoice)
    {
        m_pMasteringVoice->DestroyVoice();
        m_pMasteringVoice = nullptr;
    }

    if (m_pXAudio2)
    {
        m_pXAudio2->Release();
        m_pXAudio2 = nullptr;
    }

    // Media Foundation終了
    MFShutdown();

    // COM終了
    CoUninitialize();
}

