#pragma once

//警告についてのｺｰﾄﾞ分析を無効にする.4005:再定義.
#pragma warning(disable:4005)
#include <Windows.h>
#undef min			//std::minと競合するのでwindows.hのminを無効化.
#undef max			//std::maxと競合するのでwindows.hのmaxを無効化.
					//NOMINMAXだと競合する
#include <crtdbg.h>

//-----授業コード後に追加
#include <cmath>
#include <iostream>
#include <algorithm>	//std::min,std::maxを使用するので追加.

//DirectX9
#include <d3dx9.h>
//DirectX11
#include <D3DX11.h>
#include <D3D11.h>
//DirectX10
#include <D3DX10.h>	//「D3DX～」の定義使用時に必要.
#include <D3D10.h>

#include "MyMacro.h"

//ライブラリ読み込み.
#pragma comment( lib, "winmm.lib" )
//DirectX9
#pragma comment( lib, "d3dx9.lib" )
#pragma comment( lib, "d3d9.lib" )
//DirectX11
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3d11.lib" )
//DirectX10
#pragma comment( lib, "d3dx10.lib" )	//「D3DX～」の定義使用時に必要.

//=================================================
//	定数.
//=================================================
//#define WND_W 1280	//←定数宣言で#defineは使わない.
const int   WND_W	= 1920;		//ウィンドウの幅.
const float WND_WF	= 1920.f;	//ウィンドウの幅.
const int   WND_H	= 1080;		//ウィンドウの高さ.
const float WND_HF	= 1080.f;	//ウィンドウの高さ.
const int	FPS		= 60;		//フレームレート.

//-----定数宣言(連続入力阻止カウントマックス)-----.
const int INPUT_COUNT_MAX = 60;	//連続入力阻止のカウントマックス.

//いったん仮でプレイヤーだけの数にしておく
//プレイヤーの数
static constexpr int PLAYER_MAX = 4;
static const int ShotMax = 100;	// 弾の最大数

//アイテムの数.
static constexpr int ITEM_MAX = 6;

//テスト.
const int IMAGE = 4;

//=================================================
//  列挙型.
//=================================================
//列挙.
enum BulletKinds
{
	Mesh_1 = 0,
	Mesh_2,
	Mesh_3,
	Mesh_4,

	Max
};


//=================================================
//	構造体
//=================================================
//カメラ情報構造体.
struct CAMERA
{
	D3DXVECTOR3	vPosition;	//視点.
	D3DXVECTOR3 vLook;		//注視点.
};
//ライト情報.
struct LIGHT
{
	D3DXVECTOR3	Position;	//位置.
	D3DXVECTOR3	vDirection;	//方向.
	D3DXMATRIX	mRotation;	//回転行列.
	float		fIntensity;	//強度(明るさ).
	float		Range;		//位置,影響半径
	D3DXVECTOR3	Color;
	D3DXVECTOR3 Atten;		//減衰
};


//タンクの情報構造体.
struct TankTuning
{
	float moveSpeed = 0.1f;			//前進、後退
	float bodyTurnSpeed = 0.01f;	//車体ヨー
	float turretTurnSpeed = 0.01f;	//砲塔ヨー
	float cannonHeight = 0.3f;		//砲塔の取り付けの高さ
};

//アイテム効果の構造体.
struct ItemInfomation
{
	bool	m_ShieldFlag;				//シールドを張っているかどうか.
	float	m_Speed;					//プレイヤーの速度変更用.
	float	m_Power;					//プレイヤーの攻撃変更用.
	float	m_Blast;					//プレイヤーの爆風増加変更用.
	bool	m_Reflection;				//プレイヤーの反射変更用.
	float	m_Reload;					//プレイヤーのリロード間隔変更用.

	//コンストラクタ.
	ItemInfomation()
		: m_ShieldFlag		(false)
		, m_Speed			(0.0f)
		, m_Power			(0.0f)
		, m_Blast			(0.0f)
		, m_Reflection		(false)
		, m_Reload			(0.0f)
	{
	}
};
