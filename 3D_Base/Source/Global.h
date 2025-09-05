#pragma once

//警告についてのｺｰﾄﾞ分析を無効にする.4005:再定義.
#pragma warning(disable:4005)
#include <Windows.h>
#include <crtdbg.h>

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
const float WND_W = 1920;	//ウィンドウの幅.
const float WND_H = 1080;	//ウィンドウの高さ.
const int FPS = 60;			//フレームレート.

//-----定数宣言(連続入力阻止カウントマックス)-----.
const int INPUT_COUNT_MAX = 60;	//連続入力阻止のカウントマックス.

//いったん仮でプレイヤーだけの数にしておく
//プレイヤーの数
static constexpr int PLAYER_MAX = 4;
static const int ShotMax = 100;	// 弾の最大数



//アイテムの数.
static constexpr int ITEM_MAX = 6;



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
};