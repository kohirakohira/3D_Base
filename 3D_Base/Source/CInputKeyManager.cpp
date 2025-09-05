#include "CInputKeyManager.h"

CInputKeyManager::CInputKeyManager()
	: Input			( false )		//false: , true: .
	, OldInput		( false )		//false: , true: .
	, CheckKey		( ' ' )		//中に入るのは、なんのキーか.
{

}

CInputKeyManager::~CInputKeyManager()
{

}

void CInputKeyManager::Update()
{
	//今の入力を保存する.
	OldInput = Input;
	//今押されているかどうか.
	//ビットマスクの結果が0なら押されていない(false),0じゃないなら押されている(true).
	Input = (GetAsyncKeyState(CheckKey) & 0x8000) != 0;
}
void CInputKeyManager::Init()
{
	//初期化してあげる.
	Input		= false;
	OldInput	= false;
	CheckKey	= ' ';
}

//押された瞬間.
bool CInputKeyManager::NowInputKey()
{
	//前フレーム押されていなかったら.
	if (Input == true && OldInput == false)
	{
		return true;
	}
	return false;
}

//押されている間.
bool CInputKeyManager::InputKey()
{
	//押された瞬間を返す.
	return Input;
}

//離した瞬間.
bool CInputKeyManager::ReleseInputKey()
{
	//今押されていなかったら.
	if (Input == false && OldInput == true)
	{
		return true;
	}
	return false;
}
