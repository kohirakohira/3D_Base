#pragma once

#include "GameObject//StaticMeshObject//CStaticMeshObject.h"

/*********************************************************
*	�ǃN���X
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