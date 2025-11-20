#include "CTitleOnlyPlayer.h"

CTitleOnlyPlayer::CTitleOnlyPlayer()
{

}

CTitleOnlyPlayer::~CTitleOnlyPlayer()
{

}

//çXêVä÷êî.
void CTitleOnlyPlayer::Update()
{

}

//ï`âÊä÷êî.
void CTitleOnlyPlayer::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	CCharacter::Draw(View, Proj, Light, Camera);
}