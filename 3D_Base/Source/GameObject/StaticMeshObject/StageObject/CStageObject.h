#pragma once
//-----継承するクラス-----
#include "GameObject//StaticMeshObject//CStaticMeshObject.h"

#include "Collision/Collider/BoxCollider/CBoxCollider.h"

#include <memory>
/*********************************************************
*	ステージオブジェクトクラス
**/
class CStageObject
	: public CStaticMeshObject
{
public:
	CStageObject();
	virtual ~CStageObject();

	void Update() override;

	//BoxColliderの取得
	std::shared_ptr<CBoxCollider> GetBoxCollider() const { return m_pBoxCollider; }

private:
	std::shared_ptr<CBoxCollider> m_pBoxCollider;
};