#pragma once

//キャラクターオブジェクトクラス
#include "GameObject/StaticMeshObject/Character/CharacterObject/CCharacterObject.h"

//COM共通ヘッダー
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/ComCommon/ComCommon.h"

//ライブラリ
#include <memory>

class CComUtility
{
public:
	//一番近い敵候補
	static std::shared_ptr<CCharacterObjectBase>

	FindNearestEnemy(const ComWorldContext& world,
		const	CCharacterObjectBase& self,
		int		selfId,
		float&	outDist2);

    //分離ベクトル
    static void ComputeSeparation(const ComWorldContext& world,
        const CCharacterObjectBase& self,
        float avoidRadius,
        D3DXVECTOR3& outSep,
        float& outNearest);

    //危険ゾーン判定
    static bool IsInDangerZone(const ComWorldContext& world,
        const D3DXVECTOR3& pos,
        float selfRadius);
};