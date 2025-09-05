#include "CSoundManager.h"
#include "MyMacro.h"

CSoundManager::CSoundManager()
	: m_pSound	()
{
	//�C���X�^���X����.
	for( int i = 0; i < enList::max; i++ )
	{
		m_pSound[i] = new CSound();
	}
}

CSoundManager::~CSoundManager()
{
	Release();

	//�C���X�^���X�j��.
	for( int i = enList::max - 1; i >= 0; i-- )
	{
		SAFE_DELETE( m_pSound[i] );
	}
}

//�T�E���h�f�[�^�Ǎ��֐�.
bool CSoundManager::Load( HWND hWnd )
{
	struct SoundList
	{
		int listNo;				//enList�񋓌^��ݒ�.
		const TCHAR path[256];	//�t�@�C���̖��O(�p�X�t��).
		const TCHAR alias[32];	//�G�C���A�X��.
	};
	SoundList SList[] =
	{
		{ enList::SE_Jump,		_T("Data\\Sound\\SE\\Jump.wav"),			_T("SE_Jump")	},
		{ enList::BGM_Bonus,	_T("Data\\Sound\\BGM\\BonusGameHouse.mp3"),	_T("BGM_Bonus")	},
		{ enList::SE_Clear,		_T("Data\\Sound\\SE\\Clear.wav"),			_T("SE_Clear")	},
	};
	//�z��̍ő�v�f�����Z�o (�z��S�̂̃T�C�Y/�z��1���̃T�C�Y).
	int list_max = sizeof( SList ) / sizeof( SList[0] );
	for( int i = 0; i < list_max; i++ )
	{
		if( m_pSound[SList[i].listNo]->Open(
			SList[i].path,
			SList[i].alias,
			hWnd ) == false )
		{
			return false;
		}
	}

	return true;
}

//�T�E���h�f�[�^����֐�.
void CSoundManager::Release()
{
	//�J�������Ƌt���ŕ���.
	for( int i = enList::max - 1; i >= 0; i-- )
	{
		if( m_pSound[i] != nullptr )
		{
			m_pSound[i]->Close();
		}
	}
}
