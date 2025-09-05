#pragma once

//�x���ɂ��Ă̺��ޕ��͂𖳌��ɂ���.4005:�Ē�`.
#pragma warning(disable:4005)
#include <Windows.h>
#include <crtdbg.h>

//DirectX9
#include <d3dx9.h>
//DirectX11
#include <D3DX11.h>
#include <D3D11.h>
//DirectX10
#include <D3DX10.h>	//�uD3DX�`�v�̒�`�g�p���ɕK�v.
#include <D3D10.h>

#include "MyMacro.h"

//���C�u�����ǂݍ���.
#pragma comment( lib, "winmm.lib" )
//DirectX9
#pragma comment( lib, "d3dx9.lib" )
#pragma comment( lib, "d3d9.lib" )
//DirectX11
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3d11.lib" )
//DirectX10
#pragma comment( lib, "d3dx10.lib" )	//�uD3DX�`�v�̒�`�g�p���ɕK�v.

//=================================================
//	�萔.
//=================================================
//#define WND_W 1280	//���萔�錾��#define�͎g��Ȃ�.
const int WND_W = 1280;	//�E�B���h�E�̕�.
const int WND_H = 720;	//�E�B���h�E�̍���.
const int FPS = 60;		//�t���[�����[�g.


//=================================================
//	�\����
//=================================================
//�J�������\����.
struct CAMERA
{
	D3DXVECTOR3	vPosition;	//���_.
	D3DXVECTOR3 vLook;		//�����_.
};
//���C�g���.
struct LIGHT
{
	D3DXVECTOR3	Position;	//�ʒu.
	D3DXVECTOR3	vDirection;	//����.
	D3DXMATRIX	mRotation;	//��]�s��.
	float		fIntensity;	//���x(���邳).
};
