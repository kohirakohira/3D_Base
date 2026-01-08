#pragma once
#include <iostream>
#include <vector>

//-----外部クラス-----
#include "../../../../Camera/CCamera.h" //カメラクラス.
#include "../../../../Assets/BackGround/BackGround.h"	//背景クラス.
#include "../../../../GameObject/SpriteObject/CSpriteObject.h"	//スプライトオブジェクトクラス.
#include "../../../../Assets/Data/Singleton/GameDataManager/CGameDataManager.h"	//ゲームデータクラス.
#include "../../../../GameObject/UI/CUIObject/NumberImage/NumberImage.h"	//数字クラス.
#include "../../../../GameObject/UI/CUIObject/CUIObject.h"	//UIクラス.
#include "../../../../Global.h"	//グローバル.
//キャラクターマネージャー.
#include "../../../../GameObject/StaticMeshObject/Character/CharacterObject/Player/CharacterManager/CCharacterManager.h"

//-----メッシュクラス-----
#include "../../../../Assets/Mesh/StaticMesh/CStaticMesh.h" //スタティックメッシュクラス.
#include "../../../../Assets/Sprite/Sprite3D/CSprite3D.h"	//スプライト3D.
#include "../../../../Assets/Sprite/Sprite2D/CSprite2D.h"	//スプライト2D.
//-----エフェクト-----
#include "../../../../Assets/Effect/CEffect.h"


//======================================================
//	リザルトの演出.
//======================================================
class CResultProduction
{
public: //キャラクターの位置を設定する用の構造体. 
	struct PLAYER_POS 
	{ 
		D3DXVECTOR3 OnePos; 
		D3DXVECTOR3 TwoPos; 
		D3DXVECTOR3 TreePos; 
		D3DXVECTOR3 Fouros; 
		PLAYER_POS() 
			: OnePos ({ 0.0f, 0.0f, 0.0f }) 
			, TwoPos ({ 0.0f, 0.0f, 0.0f }) 
			, TreePos ({ 0.0f, 0.0f, 0.0f }) 
			, Fouros ({ 0.0f, 0.0f, 0.0f }) 
		{ } 
	};
private:
	//1つのエフェクトを管理する構造体.
	struct EffectManagement
	{
		//再生用ハンドル.
		::EsHandle handle;
		//キャラ位置.
		D3DXVECTOR3 offset;
	};

	//キャラ1人分のエフェクト演出管理.
	struct AllEffects
	{
		//キャラの持つエフェクトのコンテナ.
		std::vector<EffectManagement> effects;
	};
	//リザルトの花火の演出データ.
	std::vector<AllEffects> m_FireworkEffects;
	//リザルトの全キャラ分の演出データ.
	std::vector<AllEffects> m_CharacterEffects;

public:
	CResultProduction();
	~CResultProduction();

	//生成関数.
	void Create();
	//初期化.
	void Init(DrawResult result);
	//読み込み関数.
	HRESULT LoadData();

	//勝ったか引き分けか.
	void SetIsJudge(bool j) { m_IsJudge = j; }

	//引き分け時の位置設定.
	void SetPositionJudge(const DrawResult& result);

	//キャラクターの位置設定用.
	void SetPositionRanking();

	//キャラクターのX座標を決める関数.
	std::vector<float> CalcCenterPosition(int count, float spacing);

	//カメラのインスタンス取得.
	std::shared_ptr<CCamera> const GetCamera() { return m_Camera; }

public:
	//勝ち抜け.
	void WinUpdate();
	void WinDraw();
	//引き分け.
	void DrawUpdate();
	void DrawDraw();

//private:
//	//エフェクト.
//	::EsHandle hEffect_FIRE[4];
//	::EsHandle hEffect_SMOKE[PLAYER_MAX][2];

private:
	//キャラクターの基準位置.
	float m_CharaPosX;

	//勝ちか引き分け判定用.
	bool m_IsJudge;

	//時間.
	float m_Timer;

	//引き分け時構造体.
	DrawResult m_Result;

	//プレイヤーの位置. 
	PLAYER_POS m_StagingPosition;

	//カメラクラス変数.
	std::shared_ptr<CCamera>		m_Camera;

	//スタティックメッシュ.
	std::vector<std::shared_ptr<CStaticMesh>>	m_BodyMesh;
	std::vector<std::shared_ptr<CStaticMesh>>	m_CannonMesh;
	//スプライト3D.
	std::shared_ptr<CSprite3D>		m_SpriteGround;
	std::shared_ptr<CSprite3D>		m_SpriteBackGround;
	//スプライト2D.
	std::shared_ptr<CSprite2D>		m_SpriteNumber;
	std::shared_ptr<CSprite2D>		m_SpriteKillUI;
	std::array<std::shared_ptr<CSprite2D>, PLAYER_MAX>		m_SpritePlayerUI;

	//キャラクター番号.
	std::array<std::unique_ptr<CUIObject>, PLAYER_MAX>		m_PlayerUI;
	//数字クラス.
	std::unique_ptr<NumberImage>	m_Number;
	//UIクラス.
	std::unique_ptr<CUIObject>		m_KillUI;
	//スプライトオブジェクト.
	//背景クラス変数.
	std::unique_ptr<BackGround>			m_BackGround;
	std::unique_ptr<CSpriteObject>		m_SpriteObjGround;
	std::shared_ptr<CCharacterManager>	m_CharacterManager;
};