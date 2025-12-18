#pragma once
//-----ライブラリ-----
#include <map>

//-----外部クラス-----
#include "CSound.h" // サウンドクラス
#include "Assets//Sound//FileReader//CFileReader.h" // ファイルローダークラス

class CSoundManager
{
public: // 構造体
    enum SE // 効果音 
    {
        SE_QUACK,    // サンプル1
        SE_CLEAR,    // サンプル2
        BGM_sample,  // サンプル3

        MAX
    };

public:
    static CSoundManager& GetInstance()
    {
        static CSoundManager instance;
        return instance;
    }

    bool Load();
    void Release();

    static void PlaySE(SE id)
    {
        GetInstance().m_SoundMap[id]->PlaySE();
    }

    static void PlayLoop(SE id)
    {
        GetInstance().m_SoundMap[id]->PlayLoop();
    }

    static void Stop(SE id)
    {
        GetInstance().m_SoundMap[id]->Stop();
    }

private:
    CSoundManager();
    ~CSoundManager();

    // コピーと代入を禁止
    // 名前はただの識別子なので、使わないなら書かなくてもよい
    CSoundManager(const CSoundManager&) = delete;
    CSoundManager& operator=(const CSoundManager&) = delete;

private:
    IXAudio2* m_pXAudio2;
    IXAudio2MasteringVoice* m_pMasteringVoice;


    std::map<SE, CSound*> m_SoundMap;
};
