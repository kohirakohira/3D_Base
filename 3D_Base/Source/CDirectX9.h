#pragma once

//�x���ɂ��ẴR�[�h���͂𖳌��ɂ���.4005:�Ē�`.
#pragma warning(disable:4005)

//DirectX9.
#include <d3dx9.h>

//���C�u�����ǂݍ���.
#pragma comment( lib, "winmm.lib" )
//DirectX9.
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )


/**************************************************
*	DirectX9 �Z�b�g�A�b�v.
**/
class CDirectX9
{
public:
	CDirectX9();
	~CDirectX9();

	//DirectX9�\�z.
	HRESULT Create( HWND hWnd );
	//DirectX9���.
	void Release();

	//�f�o�C�X�I�u�W�F�N�g���擾.
	LPDIRECT3DDEVICE9 GetDevice() const { return m_pDevice9; }


private:
	LPDIRECT3DDEVICE9	m_pDevice9;	//�f�o�C�X�I�u�W�F�N�g.
};