#pragma once
//-----ライブラリ-----
#include <map>

//-----外部クラス-----
#include "CSound.h" // サウンドクラス
#include "Assets//Sound//FileReader//CFileReader.h" // ファイルローダークラス

class CSoundManager
{
public: // 構造体
    enum SoundList // 音のリスト 
    {
        BGM_Title,		//タイトルステージ.
        BGM_Main,		//ゲームメイン.
        BGM_Result_Win,	//リザルト(勝利)
        BGM_Result_Draw,//リザルト(引き分け)

        SE_Select,		//選択肢.
        SE_Click,		//決定.
        SE_Connect,		//コントローラー接続.
        SE_UnConnect,	//コントローラーの接続が切れる.

        SE_GameStart,	//ゲームの開始.
        SE_GameEnd,		//ゲームの終了.
        SE_FireWork,	//花火.
        SE_Spark,		//火花.
        SE_Door,		//シャッター.

        SE_Shot,		//発射.
        SE_Damage,		//ダメージ.
        SE_Explosion,	//爆発.
        SE_Impact,		//衝突.

        SE_Move1,		//1P:戦車の移動音.
        SE_Move2,		//2P:戦車の移動音.
        SE_Move3,		//3P:戦車の移動音.
        SE_Move4,		//4P:戦車の移動音.

        //音が増えたら「ここ」に追加してください.
        max,		//最大数.
    };

public:
    static CSoundManager& GetInstance()
    {
        static CSoundManager instance;
        return instance;
    }

    bool Load();
    void Release();

    static void PlaySE(SoundList id)
    {
        GetInstance().m_SoundMap[id]->PlaySE();
    }

    static void PlayLoop(SoundList id)
    {
        GetInstance().m_SoundMap[id]->PlayLoop();
    }

    static void Stop(SoundList id)
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


    std::map<SoundList, CSound*> m_SoundMap;
};
