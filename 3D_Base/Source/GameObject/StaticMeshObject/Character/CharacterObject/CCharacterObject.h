#pragma once
//STL.
#include <iostream>

//継承するクラス.
#include "GameObject/StaticMeshObject/Character/CCharacter.h"

//=========================================================
//	キャラクターオブジェクト※基底クラス(キャラクターを継承).
//=========================================================
class CCharacterObject
	: public CCharacter
{
public:
	CCharacterObject();
	virtual ~CCharacterObject() = 0;

private:

};