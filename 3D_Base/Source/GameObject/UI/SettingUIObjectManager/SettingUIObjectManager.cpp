#include "SettingUIObjectManager.h"

SettingUIObjectManager::SettingUIObjectManager()
{
}

SettingUIObjectManager::~SettingUIObjectManager()
{
}

void SettingUIObjectManager::Update()
{

}
//描画処理.
void SettingUIObjectManager::Draw()
{

}

//インスタンス生成.
void SettingUIObjectManager::Create()
{
	
}

//スプライトの保存関数.
void SettingUIObjectManager::SetSettingUIImg(SettingKinds kind, std::shared_ptr<CSprite2D> sprite)
{
	//中身無かったら返す.
	if (sprite == nullptr)
	{
		return;
	}
	m_Sprites[kind] = sprite;
}