#include "CMultiInputKeyManager.h"

CMultiInputKeyManager::CMultiInputKeyManager()
	: m_Key				()
{

}

CMultiInputKeyManager::~CMultiInputKeyManager()
{
	
}

//í‚É‰ñ‚µ‚Ä‚Ù‚µ‚¢.
void CMultiInputKeyManager::Update()
{
	//m_Key‚ÍAstd::unodere_map<char, CInputKeyManager>‚Ìî•ñ‚ğ•Û‚µ‚Ä‚¢‚é.
	for (auto& pair : m_Key)
	{
		auto& Inkey		= pair.first;	//m_Key‚Ì1‚Â–Ú‚Ì—v‘f:char.
		auto& manager	= pair.second;	//m_Key‚Ì2‚Â–Ú‚Ì—v‘f:CInputKeyManager.

		//ƒL[•ªUpdate()‚ğ‰ñ‚·.
		manager.Update();
	}
}

//‰Šú‰».
void CMultiInputKeyManager::Init()
{
	for (auto& pair : m_Key)
	{
		auto& Inkey = pair.first;	//m_Key‚Ì1‚Â–Ú‚Ì—v‘f:char.
		auto& manager = pair.second;	//m_Key‚Ì2‚Â–Ú‚Ì—v‘f:CInputKeyManager.

		//ƒL[•ªInit()‚ğ‰ñ‚·.
		manager.Init();
	}
}

//•K—v‚ÈƒL[•ªİ’è‚µ‚Ä‚ ‚°‚é.
void CMultiInputKeyManager::SetKey(const std::vector<char>& keys)
{
	for (char Inkey : keys)
	{
		if (m_Key.find(Inkey) == m_Key.end())
		{
			//V‚µ‚¢—v‘f‚Ì’Ç‰Á¦.insert(std::make_pair(Œ^,@’l))‚à“¯—l.
			CInputKeyManager manager;
			manager.SetKey(Inkey);
			m_Key.emplace(Inkey, manager);
		}
	}
}

//‰Ÿ‚³‚ê‚½uŠÔ.
bool CMultiInputKeyManager::NowInputKey(char key)
{
	//“o˜^Ï‚İ?.
	auto It = m_Key.find(key);

	//‘S‚Ä“o˜^‚Å‚«‚Ä‚éH.
	if (It != m_Key.end())
	{
		//‘Sƒ{ƒ^ƒ“‚ğ”»’è‚³‚¹‚é.
		return It->second.NowInputKey();
	}

	//–¢“o˜^‚Ífalse‚Å•Ô‚·.
	return false;
}

//‰Ÿ‚³‚ê‚Ä‚¢‚éŠÔ.
bool CMultiInputKeyManager::InputKey(char key)
{
	//“o˜^Ï‚İ?.
	auto It = m_Key.find(key);

	//‘S‚Ä“o˜^‚Å‚«‚Ä‚éH.
	if (It != m_Key.end())
	{
		//‘Sƒ{ƒ^ƒ“‚ğ”»’è‚³‚¹‚é.
		return It->second.InputKey();
	}
	//–¢“o˜^‚Ífalse‚Å•Ô‚·.
	return false;
}

//—£‚µ‚½uŠÔ.
bool CMultiInputKeyManager::ReleaseInputKey(char key)
{
	//“o˜^Ï‚İ?.
	auto It = m_Key.find(key);

	//‘S‚Ä“o˜^‚Å‚«‚Ä‚éH.
	if (It != m_Key.end())
	{
		//‘Sƒ{ƒ^ƒ“‚ğ”»’è‚³‚¹‚é.
		return It->second.ReleseInputKey();
	}
	//–¢“o˜^‚Ífalse‚Å•Ô‚·.
	return false;
}
