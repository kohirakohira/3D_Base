#pragma once

//キャラクターオブジェクト
#include "GameObject/StaticMeshObject/Character/CharacterObject/CCharacterObject.h"

//アイテム
#include "GameObject/StaticMeshObject/ItemBoxManager/ItemBox/CItemBox.h"

//ライブラリ
#include <vector>
#include <memory>

//COM共通ヘッダー

//障害物
struct SimpleObstacle
{
	D3DXVECTOR3 pos;
	float radius;
};

struct ComWorldContext
{
	//全プレイヤー
	const std::vector<std::shared_ptr<CCharacterObjectBase>>* players = nullptr;

	//アイテム
	const std::vector<std::shared_ptr<CItemBox>>* items = nullptr;

};