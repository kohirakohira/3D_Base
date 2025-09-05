/***************************************************************************************************
*	SkinMeshCode Version 2.50
*	LastUpdate	: 2025/06/23.
**/
#pragma once
//�x���ɂ��ẴR�[�h���͂𖳌��ɂ���B4005�F�Ē�`.
#pragma warning( disable : 4005 )

#include "CSkinMeshParser.h"

//�O���錾.
class CDirectX9;
class CDirectX11;

/**************************************************
*	�X�L�����b�V���N���X
*
*
**/
class CSkinMesh
{
public:
	//======================================
	//	�\����.
	//======================================
	//�R���X�^���g�o�b�t�@�̃A�v�����̒�`.
	//���V�F�[�_���̃R���X�^���g�o�b�t�@�ƈ�v���Ă���K�v����.
	//���b�V���P��.
	struct CBUFFER_PER_MESH
	{
		D3DXMATRIX	mW;			//���[���h�s��.
		D3DXMATRIX	mWVP;		//���[���h����ˉe�܂ł̕ϊ��s��.
	};

	//�}�e���A���P��.
	struct CBUFFER_PER_MATERIAL
	{
		D3DXVECTOR4 Ambient;	//�����i�A���r�G���g�j.
		D3DXVECTOR4 Diffuse;	//�g�U���ˌ��i�f�B�t���[�Y�j.
		D3DXVECTOR4 Specular;	//���ʔ��ˌ��i�X�y�L�����j.
	};

	//�V�F�[�_�[�ɓn���l.
	struct CBUFFER_PER_FRAME
	{
		D3DXVECTOR4 CameraPos;		//�J�����ʒu.
		D3DXVECTOR4 vLightDir;		//���C�g����.
	};

	//�{�[���P��.
	struct CBUFFER_PER_BONES
	{
		D3DXMATRIX mBone[D3DXPARSER::MAX_BONES];
		CBUFFER_PER_BONES()
		{
			for ( int i = 0; i < D3DXPARSER::MAX_BONES; i++ )
			{
				D3DXMatrixIdentity( &mBone[i] );
			}
		}
	};

	//���_�\����.
	struct VERTEX
	{
		D3DXVECTOR3	Position;	//���_�ʒu.
		D3DXVECTOR3	vNormal;	//���_�@��.
		D3DXVECTOR2	Texture;	//UV���W.
		UINT BoneIndex[4];		//�{�[�� �ԍ�.
		float BoneWeight[4];	//�{�[�� �d��.
		VERTEX()
			: Position		()
			, vNormal		()
			, Texture		()
			, BoneIndex		()
			, BoneWeight	()
		{}
	};

public:
	CSkinMesh();	//�R���X�g���N�^.
	~CSkinMesh();	//�f�X�g���N�^.

	HRESULT Init( CDirectX9& pDx9, CDirectX11& pDx11, LPCTSTR FileName );

	//����֐�.
	HRESULT Release();

	//�`��֐�.
	void Render( const D3DXMATRIX& mView, 
		const D3DXMATRIX& mProj, 
		const LIGHT& Light,
		const D3DXVECTOR3& CamPos,
		const LPD3DXANIMATIONCONTROLLER pAC );	//nullptr�Ńf�t�H���g�g�p����.

	double GetAnimSpeed()				{ return m_AnimSpeed;		}
	void SetAnimSpeed( double Speed )	{ m_AnimSpeed = Speed;		}

	double GetAnimTime()				{ return m_AnimTime;		}
	void SetAnimTime( double Time )		{ m_AnimTime = Time;		}

	//�p�[�T�[�N���X����A�j���[�V�����R���g���[�����擾����.
	LPD3DXANIMATIONCONTROLLER GetAnimationController() { return m_pD3dxMesh->m_pAnimController; }

	//�A�j���[�V�����Z�b�g�̐؂�ւ�.
	void ChangeAnimSet( int index, LPD3DXANIMATIONCONTROLLER pAC );
	//�A�j���[�V�����Z�b�g�̐؂�ւ�(�J�n�t���[���w��\��).
	void ChangeAnimSet_StartPos( int Index, double StartFramePos, LPD3DXANIMATIONCONTROLLER pAC );

	//�A�j���[�V������~���Ԃ��擾.
	double GetAnimPeriod( int Index );
	//�A�j���[�V���������擾.
	int GetAnimMax( LPD3DXANIMATIONCONTROLLER pAC = nullptr );

	//�w�肵���{�[�����(���W�E�s��)���擾����֐�.
	bool GetMatrixFromBone( LPCSTR BoneName, D3DXMATRIX* pOutMat );
	bool GetPosFromBone( LPCSTR BoneName, D3DXVECTOR3* pOutPos);
	bool GetDeviaPosFromBone( LPCSTR BoneName, D3DXVECTOR3* pOutPos, D3DXVECTOR3 SpecifiedPos = { 0.0f, 0.0f, 0.0f } );

	//���W����ݒ�.
	void SetPosition( const D3DXVECTOR3& vPos ) { m_Position = vPos;	}
	void SetPositionX( float x )				{ m_Position.x = x;		}
	void SetPositionY( float y )				{ m_Position.y = y;		}
	void SetPositionZ( float z )				{ m_Position.z = z;		}

	//��]����ݒ�.
	void SetRotation( const D3DXVECTOR3& vRot ) { m_Rotation = vRot;	}
	void SetRotationY( float y )				{ m_Rotation.y = y;		}
	void SetRotationX( float x )				{ m_Rotation.x = x;		}
	void SetRotationZ( float z )				{ m_Rotation.z = z;		}

	//�g�k����ݒ�E�擾.
	void SetScale( const D3DXVECTOR3& Scale )	{ m_Scale = Scale;		}
	void SetScaleX( const float x )				{ m_Scale.x = x;		}
	void SetScaleY( const float y )				{ m_Scale.x = y;		}
	void SetScaleZ( const float z )				{ m_Scale.x = z;		}

private:
	//X�t�@�C������X�L�����b�V�����쐬����.
	HRESULT LoadXMesh( LPCTSTR FileName );

	//�V�F�[�_���쐬����.
	HRESULT CreateShader();
	//�C���f�b�N�X�o�b�t�@���쐬����.
	HRESULT CreateIndexBuffer( DWORD Size, int* pIndex, ID3D11Buffer** ppIndexBuffer );

	//���b�V�����쐬����.
	HRESULT CreateAppMeshFromD3DXMesh( LPD3DXFRAME pFrame );

	//�R���X�^���g�o�b�t�@�쐬����.
	HRESULT CreateCBuffer( ID3D11Buffer** pConstantBuffer, UINT Size );
	//�T���v�����쐬����.
	HRESULT CreateSampler( ID3D11SamplerState** pSampler );

	//�S�Ẵ��b�V�����쐬����.
	void BuildAllMesh( D3DXFRAME* pFrame );

	//X�t�@�C������X�L���֘A�̏���ǂݏo���֐�.
	HRESULT ReadSkinInfo( MYMESHCONTAINER* pContainer, VERTEX* pVB, SKIN_PARTS_MESH* pParts );

	//�{�[�������̃|�[�Y�ʒu�ɃZ�b�g����֐�.
	void SetNewPoseMatrices( SKIN_PARTS_MESH* pParts, int Frame, MYMESHCONTAINER* pContainer );
	//����(���݂�)�|�[�Y�s���Ԃ��֐�.
	D3DXMATRIX GetCurrentPoseMatrix( SKIN_PARTS_MESH* pParts, int Index );

	//�t���[���`��.
	void DrawFrame( LPD3DXFRAME pFrame );
	//�p�[�c�`��.
	void DrawPartsMesh( SKIN_PARTS_MESH* pMesh, D3DXMATRIX World, MYMESHCONTAINER* pContainer );
	//���[���h�s��Z�o.
	void CalcWorldMatrix();
	//�e��V�F�[�_�ɑ���f�[�^.
	void SendCBufferPerBone( SKIN_PARTS_MESH* pMesh );
	void SendCBufferPerFrame();
	void SendCBufferPerMesh();
	void SendCBufferPerMaterial( MY_SKINMATERIAL* pMaterial );
	void SendTexture( MY_SKINMATERIAL* pMaterial );

	//�S�Ẵ��b�V�����폜.
	void DestroyAllMesh( D3DXFRAME* pFrame );
	HRESULT DestroyAppMeshFromD3DXMesh( LPD3DXFRAME p );

	//�}���`�o�C�g������Unicode�����ɕϊ�����.
	void ConvertCharaMultiByteToUnicode( WCHAR* Dest, size_t DestArraySize, const CHAR* str );

private:
	//Dx9.
	CDirectX9*				m_pDx9;
	LPDIRECT3DDEVICE9		m_pDevice9;	//Dx9�f�o�C�X�I�u�W�F�N�g.

	//Dx11.
	CDirectX11*				m_pDx11;
	ID3D11Device*			m_pDevice11;
	ID3D11DeviceContext*	m_pContext11;

	ID3D11VertexShader*		m_pVertexShader;
	ID3D11InputLayout*		m_pVertexLayout;
	ID3D11PixelShader*		m_pPixelShader;
	ID3D11SamplerState*		m_pSampleLinear;

	//�R���X�^���g�o�b�t�@.
	ID3D11Buffer*			m_pCBufferPerMesh;		//�R���X�^���g�o�b�t�@(���b�V����).
	ID3D11Buffer*			m_pCBufferPerMaterial;	//�R���X�^���g�o�b�t�@(�}�e���A����).
	ID3D11Buffer*			m_pCBufferPerFrame;		//�R���X�^���g�o�b�t�@(�t���[����).
	ID3D11Buffer*			m_pCBufferPerBone;		//�R���X�^���g�o�b�t�@(�{�[����).

	D3DXVECTOR3		m_Position;		//�ʒu(x,y,z).
	D3DXVECTOR3		m_Rotation;		//��]�l(x,y,z).
	D3DXVECTOR3		m_Scale;		//�g��k���l(x,y,z).

	D3DXMATRIX		m_mWorld;
	D3DXMATRIX		m_mRotation;

	D3DXMATRIX		m_mView;
	D3DXMATRIX		m_mProj;

	LIGHT			m_Light;
	D3DXVECTOR3		m_CamPos;

	//�A�j���[�V�������x.
	double m_AnimSpeed;
	double m_AnimTime;

	//��������p��.
//	SKIN_PARTS_MESH* m_pReleaseMaterial;

	//���b�V��.
	D3DXPARSER* m_pD3dxMesh;

	//X�t�@�C���̃p�X.
	TCHAR	m_FilePath[256];

	//�A�j���[�V�����t���[��.
	int		m_Frame;
};
