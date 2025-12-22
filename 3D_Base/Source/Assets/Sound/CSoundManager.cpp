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
    std::map<SoundList, std::wstring> filePaths =
    {
        { SoundList::BGM_Title,		    _T("Data\\Sound\\BGM\\Title_ver2.mp3"),		},
        { SoundList::BGM_Main,			_T("Data\\Sound\\BGM\\Main.mp3"),		    },
        { SoundList::BGM_Result_Win,	_T("Data\\Sound\\BGM\\Result_Win.mp3"),	    },
        { SoundList::BGM_Result_Draw,	_T("Data\\Sound\\BGM\\Result_Draw.mp3"),    },

        { SoundList::SE_Select,	    _T("Data\\Sound\\SE\\Select.mp3"),			},
        { SoundList::SE_Click,		_T("Data\\Sound\\SE\\Accept.mp3"),		    },
        { SoundList::SE_Connect,	_T("Data\\Sound\\SE\\Connect.mp3"),		    },
        { SoundList::SE_UnConnect,	_T("Data\\Sound\\SE\\UnConnect.mp3"),	    },
        { SoundList::SE_GameStart,	_T("Data\\Sound\\SE\\GameStart.mp3"),	    },
        { SoundList::SE_GameEnd,	_T("Data\\Sound\\SE\\GameEnd.mp3"),		    },
        { SoundList::SE_FireWork,	_T("Data\\Sound\\SE\\FireWork.mp3"),	    },
        { SoundList::SE_Spark,		_T("Data\\Sound\\SE\\Spark.mp3"),		    },
        { SoundList::SE_Door,		_T("Data\\Sound\\SE\\Door.mp3"),		    },
        { SoundList::SE_Shot,		_T("Data\\Sound\\SE\\Shot.mp3"),		    },
        { SoundList::SE_Damage ,	_T("Data\\Sound\\SE\\damage.mp3"),		    },
        { SoundList::SE_Explosion,	_T("Data\\Sound\\SE\\explosion_2.mp3"),	    },
        { SoundList::SE_Impact,	    _T("Data\\Sound\\SE\\Impact.mp3"),		    },
        { SoundList::SE_Move,		_T("Data\\Sound\\SE\\Move.mp3"),		    },
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

