#include "CSelectIcon.h"

CSelectIcon::CSelectIcon()
{

}

CSelectIcon::~CSelectIcon()
{
	//CUIObject‚ÌƒAƒ^ƒbƒ`‚µ‚½ƒ‚ƒm‚ğØ‚è—£‚·ŠÖ”.
	DetachSprite();
}

//“®ìŠÖ”.
void CSelectIcon::Update()
{

}

//‚±‚êˆÈãŒp³‚µ‚È‚¢.
void CSelectIcon::Draw()
{
	if (m_pSprite == nullptr) {
		return;
	}

	//•`‰æ’¼‘O‚ÅÀ•W‚â‰ñ“]î•ñ‚È‚Ç‚ğXV.
	m_pSprite->SetPosition(m_vPosition);
	m_pSprite->SetRotation(m_vRotation);
	m_pSprite->SetScale(m_vScale);

	//ƒŒƒ“ƒ_ƒŠƒ“ƒO.
	m_pSprite->Render();
}