#pragma once

#include "GameObject//StaticMeshObject//CStaticMeshObject.h"

/*********************************************************
*	•ÇƒNƒ‰ƒX
**/
class CWall
	: public CStaticMeshObject
{
public:
	CWall();
	virtual ~CWall();

	virtual void Update() override;

protected:

};