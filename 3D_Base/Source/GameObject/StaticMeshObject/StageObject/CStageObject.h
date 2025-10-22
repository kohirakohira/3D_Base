#pragma once
//-----継承するクラス-----
#include "GameObject//StaticMeshObject//CStaticMeshObject.h"

/*********************************************************
*	ステージオブジェクトクラス
**/
class CStageObject
	: public CStaticMeshObject
{
public:
	CStageObject();
	virtual ~CStageObject();
};