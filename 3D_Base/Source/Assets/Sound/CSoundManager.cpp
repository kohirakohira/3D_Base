#include "CSoundManager.h"
#include "MyMacro.h"

CSoundManager::CSoundManager()
	: m_pSound()
{
	//インスタンス生成.
	for (int i = 0; i < enList::max; i++)
	{
		m_pSound[i] = new CSound();
	}
}

CSoundManager::~CSoundManager()
{
	Release();

	//インスタンス破棄.
	for (int i = enList::max - 1; i >= 0; i--)
	{
		SAFE_DELETE(m_pSound[i]);
	}
}

//サウンドデータ読込関数.
bool CSoundManager::Load(HWND hWnd)
{
	struct SoundList
	{
		int listNo;				//enList列挙型を設定.
		const TCHAR path[256];	//ファイルの名前(パス付き).
		const TCHAR alias[32];	//エイリアス名.
	};
	SoundList SList[] =
	{
		{ enList::BGM_Title,		_T("Data\\Sound\\BGM\\Title.mp3"),			_T("BGM_Title")			},
		{ enList::BGM_Main,			_T("Data\\Sound\\BGM\\Main_ver2.mp3"),		_T("BGM_Main")			},
		{ enList::BGM_Result_Win,	_T("Data\\Sound\\BGM\\Result_Win.mp3"),		_T("BGM_Result_Win")	},
		{ enList::BGM_Result_Draw,	_T("Data\\Sound\\BGM\\Result_Draw.mp3"),	_T("BGM_Result_Draw")	},

		{ enList::SE_Select,	_T("Data\\Sound\\SE\\Select.mp3"),				_T("SE_Select")		},
		{ enList::SE_Click,		_T("Data\\Sound\\SE\\Accept.mp3"),				_T("SE_Click")		},
		{ enList::SE_Connect,	_T("Data\\Sound\\SE\\Connect.mp3"),				_T("SE_Connect")	},
		{ enList::SE_UnConnect,	_T("Data\\Sound\\SE\\UnConnect.mp3"),			_T("SE_UnConnect")	},
		{ enList::SE_GameStart,	_T("Data\\Sound\\SE\\GameStart.mp3"),			_T("SE_GameStart")	},
		{ enList::SE_GameEnd,	_T("Data\\Sound\\SE\\GameEnd.mp3"),				_T("SE_GameEnd")	},
		{ enList::SE_FireWork,	_T("Data\\Sound\\SE\\FireWork.mp3"),			_T("SE_FireWork")	},
		{ enList::SE_Spark,		_T("Data\\Sound\\SE\\Spark.mp3"),				_T("SE_Spark")		},
		{ enList::SE_Door,		_T("Data\\Sound\\SE\\Door.mp3"),				_T("SE_Door")		},
		{ enList::SE_Shot,		_T("Data\\Sound\\SE\\Shot.mp3"),				_T("SE_Shot")		},
		{ enList::SE_Damage ,	_T("Data\\Sound\\SE\\damage.mp3"),				_T("SE_Damage")		},
		{ enList::SE_Explosion,	_T("Data\\Sound\\SE\\explosion.mp3"),			_T("SE_Explosion")	},
		{ enList::SE_Impact,	_T("Data\\Sound\\SE\\Impact.mp3"),				_T("SE_Impact")		},
		{ enList::SE_Move,		_T("Data\\Sound\\SE\\Move.mp3"),				_T("SE_Move")		},
	};
	//配列の最大要素数を算出 (配列全体のサイズ/配列1つ分のサイズ).
	int list_max = sizeof(SList) / sizeof(SList[0]);
	for (int i = 0; i < list_max; i++)
	{
		if (m_pSound[SList[i].listNo]->Open(
			SList[i].path,
			SList[i].alias,
			hWnd) == false)
		{
			return false;
		}
	}

	return true;
}

//サウンドデータ解放関数.
void CSoundManager::Release()
{
	//開いた時と逆順で閉じる.
	for (int i = enList::max - 1; i >= 0; i--)
	{
		if (m_pSound[i] != nullptr)
		{
			m_pSound[i]->Close();
		}
	}
}
