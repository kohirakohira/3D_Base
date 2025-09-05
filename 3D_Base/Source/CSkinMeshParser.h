/***************************************************************************************************
*	SkinMeshCode Version 2.50
*	LastUpdate	: 2025/06/23.
**/
#pragma once

//�x���ɂ��ẴR�[�h���͂𖳌��ɂ���B4005�F�Ē�`.
#pragma warning( disable : 4005 )

#include <d3dx9.h>
#include <d3d11.h>
#include <d3dx10.h>
#include <d3dx11.h>

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

////�}�N��.
//#define SAFE_RELEASE(p)			{ if(p) { (p)->Release(); (p)=nullptr; } }
//#define SAFE_DELETE(p)			{ if(p) { delete (p);     (p)=nullptr; } }
//#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p);   (p)=nullptr; } }

////���C�g���\����.
//struct LIGHT
//{
//	D3DXVECTOR3	Position;	//�ʒu.
//	D3DXVECTOR3	vDirection;	//����.
//	D3DXMATRIX	mRotation;	//��]�s��.
//	float		Intensity;	//���x(���邳).
//};

//�I���W�i���@�}�e���A���\����.
struct MY_SKINMATERIAL
{
	TCHAR Name[110];
	D3DXVECTOR4	Ambient;			//�A���r�G���g.
	D3DXVECTOR4	Diffuse;			//�f�B�t���[�Y.
	D3DXVECTOR4	Specular;			//�X�y�L����.
	D3DXVECTOR4	Emissive;			//�G�~�b�V�u.
	float		SpecularPower;		//�X�y�L�����p���[.
	TCHAR		TextureName[512];	//�e�N�X�`���[�t�@�C����.
	ID3D11ShaderResourceView* pTexture;
	DWORD		NumFace;	//���̃}�e���A���ł���|���S����.

	MY_SKINMATERIAL()
		: Name			()
		, Ambient		()
		, Diffuse		()
		, Specular		()
		, Emissive		()
		, SpecularPower	()
		, TextureName	()
		, pTexture		( nullptr )
		, NumFace		()
	{
	}
	~MY_SKINMATERIAL()
	{
		SAFE_RELEASE( pTexture );
	}
};

//�{�[���\����.
struct BONE
{
	D3DXMATRIX	mBindPose;		//�����|�[�Y�i�����ƕς��Ȃ��j.
	D3DXMATRIX	mNewPose;		//���݂̃|�[�Y�i���̓s�x�ς��j.
	DWORD		NumChild;		//�q�̐�.
	int			ChildIndex[50];	//�����̎q��"�C���f�b�N�X"50�܂�.
	char		Name[256];

	BONE()
		: mBindPose		()
		, mNewPose		()
		, NumChild		()
		, ChildIndex	()
		, Name			()
	{
		D3DXMatrixIdentity( &mBindPose );
		D3DXMatrixIdentity( &mNewPose );
	}
};

//�p�[�c���b�V���\����.
struct SKIN_PARTS_MESH
{
	DWORD				NumVert;
	DWORD				NumFace;
	DWORD				NumUV;
	DWORD				NumMaterial;
	MY_SKINMATERIAL*	pMaterial;
	char				TextureFileName[8][256];	//�e�N�X�`���[�t�@�C����(8���܂�).
	bool				EnableTexture;

	ID3D11Buffer*	pVertexBuffer;
	ID3D11Buffer**	ppIndexBuffer;

	//�{�[��.
	int		NumBone;
	BONE*	pBoneArray;

	bool	EnableBones;	//�{�[���̗L���t���O.

	SKIN_PARTS_MESH()
		: NumVert			()
		, NumFace			()
		, NumUV				()
		, NumMaterial		()
		, pMaterial			( nullptr )
		, TextureFileName	()
		, EnableTexture		()
		, pVertexBuffer		( nullptr )
		, ppIndexBuffer		( nullptr )
		, NumBone			()
		, pBoneArray		( nullptr )
		, EnableBones		()
	{}
};

//�h���t���[���\����.
//	���ꂼ��̃��b�V���p�̍ŏI���[���h�s���ǉ�����.
struct MYFRAME
	: public D3DXFRAME
{
	D3DXMATRIX CombinedTransformationMatrix;
	SKIN_PARTS_MESH* pPartsMesh;

	MYFRAME()
		: D3DXFRAME						()
		, CombinedTransformationMatrix	()
		, pPartsMesh					( nullptr )
	{
	}
};
//�h�����b�V���R���e�i�[�\����.
//	�R���e�i�[���e�N�X�`���𕡐����Ă�悤�Ƀ|�C���^�[�̃|�C���^�[��ǉ�����
struct MYMESHCONTAINER
	: public D3DXMESHCONTAINER
{
	LPDIRECT3DTEXTURE9*	ppTextures;
	DWORD				Weight;				//�d�݂̌��i�d�݂Ƃ͒��_�ւ̉e���B�j.
	DWORD				BoneNum;			//�{�[���̐�.
	LPD3DXBUFFER		pBoneBuffer;		//�{�[���E�e�[�u��.
	D3DXMATRIX**		ppBoneMatrix;		//�S�Ẵ{�[���̃��[���h�s��̐擪�|�C���^.
	D3DXMATRIX*			pBoneOffsetMatrices;//�t���[���Ƃ��Ẵ{�[���̃��[���h�s��̃|�C���^.

	MYMESHCONTAINER()
		: D3DXMESHCONTAINER		()
		, ppTextures			( nullptr )
		, Weight				()
		, BoneNum				()
		, pBoneBuffer			( nullptr )
		, ppBoneMatrix			( nullptr )
		, pBoneOffsetMatrices	( nullptr )
	{}
};
//X�t�@�C�����̃A�j���[�V�����K�w��ǂ݉����Ă����N���X��h��������.
//	ID3DXAllocateHierarchy�͔h�����邱�Ƒz�肵�Đ݌v����Ă���.
class MY_HIERARCHY
	: public ID3DXAllocateHierarchy
{
public:
	MY_HIERARCHY()
		: ID3DXAllocateHierarchy(){}
	STDMETHOD( CreateFrame )( THIS_ LPCSTR, LPD3DXFRAME * );
	STDMETHOD( CreateMeshContainer )( THIS_ LPCSTR, CONST D3DXMESHDATA*, CONST D3DXMATERIAL*,
		CONST D3DXEFFECTINSTANCE*, DWORD, CONST DWORD *, LPD3DXSKININFO, LPD3DXMESHCONTAINER * );
	STDMETHOD( DestroyFrame )( THIS_ LPD3DXFRAME );
	STDMETHOD( DestroyMeshContainer )( THIS_ LPD3DXMESHCONTAINER );
};

//==================================================================================================
//
//	�p�[�T�[�N���X.
//
//==================================================================================================
class D3DXPARSER
{
public:
	//�ő�{�[����.
	static constexpr int MAX_BONES	= 255;
	//�ő�A�j���[�V�����Z�b�g��.
	static constexpr int MAX_ANIM_SET = 100;

public:
	D3DXPARSER();
	~D3DXPARSER();

	HRESULT LoadMeshFromX( LPDIRECT3DDEVICE9, LPCTSTR fileName );
	HRESULT AllocateBoneMatrix( LPD3DXMESHCONTAINER );
	HRESULT AllocateAllBoneMatrices( LPD3DXFRAME );
	VOID UpdateFrameMatrices( LPD3DXFRAME, LPD3DXMATRIX );

	//--------------------------------------.
	//	�擾�֐�.
	//--------------------------------------.
	int GetNumVertices( MYMESHCONTAINER* pContainer );
	int GetNumFaces( MYMESHCONTAINER* pContainer );
	int GetNumMaterials( MYMESHCONTAINER* pContainer );
	int GetNumUVs( MYMESHCONTAINER* pContainer );
	int GetNumBones( MYMESHCONTAINER* pContainer );
	int GetNumBoneVertices( MYMESHCONTAINER* pContainer, int BoneIndex );
	DWORD GetBoneVerticesIndices( MYMESHCONTAINER* pContainer, int BoneIndex, int IndexInGroup );
	double GetBoneVerticesWeights( MYMESHCONTAINER* pContainer, int BoneIndex, int IndexInGroup );
	D3DXVECTOR3 GetVertexCoord( MYMESHCONTAINER* pContainer, DWORD Index );
	D3DXVECTOR3 GetNormal( MYMESHCONTAINER* pContainer, DWORD Index );
	D3DXVECTOR2 GetUV( MYMESHCONTAINER* pContainer, DWORD Index );
	int GetIndex( MYMESHCONTAINER* pContainer, DWORD Index );
	D3DXVECTOR4 GetAmbient( MYMESHCONTAINER* pContainer, int Index );
	D3DXVECTOR4 GetDiffuse( MYMESHCONTAINER* pContainer, int Index );
	D3DXVECTOR4 GetSpecular( MYMESHCONTAINER* pContainer, int Index );
	D3DXVECTOR4 GetEmissive( MYMESHCONTAINER* pContainer, int Index );
	LPSTR GetTexturePath( MYMESHCONTAINER* pContainer, int Index );
	float GetSpecularPower( MYMESHCONTAINER* pContainer, int Index );
	int GeFaceMaterialIndex( MYMESHCONTAINER* pContainer, int FaceIndex );
	int GetFaceVertexIndex( MYMESHCONTAINER* pContainer, int FaceIndex, int IndexInFace );
	D3DXMATRIX GetBindPose( MYMESHCONTAINER* pContainer, int BoneIndex );
	D3DXMATRIX GetNewPose( MYMESHCONTAINER* pContainer, int BoneIndex );
	LPCSTR GetBoneName( MYMESHCONTAINER* pContainer, int BoneIndex );

	//���b�V���R���e�i���擾����.
	LPD3DXMESHCONTAINER GetMeshContainer( LPD3DXFRAME pFrame );

	//�A�j���[�V�����Z�b�g�̐؂�ւ�.
	void ChangeAnimSet( int Index, LPD3DXANIMATIONCONTROLLER pAC );
	//�A�j���[�V�����Z�b�g�̐؂�ւ�(�J�n�t���[���w��\��).
	void ChangeAnimSet_StartPos( int Index, double StartFramePos, LPD3DXANIMATIONCONTROLLER pAC );

	//�A�j���[�V������~���Ԃ��擾.
	double GetAnimPeriod( int Index );
	//�A�j���[�V���������擾.
	int GetAnimMax( LPD3DXANIMATIONCONTROLLER pAC );

	//�w�肵���{�[�����(���W�E�s��)���擾����֐�.
	bool GetMatrixFromBone( LPCSTR BoneName, D3DXMATRIX* pOutMat );
	bool GetPosFromBone( LPCSTR BoneName, D3DXVECTOR3* pOutPos );

	//�ꊇ�������.
	HRESULT Release();

public:
	MY_HIERARCHY	cHierarchy;
	MY_HIERARCHY*	m_pHierarchy;
	LPD3DXFRAME		m_pFrameRoot;

	LPD3DXANIMATIONCONTROLLER	m_pAnimController;			//�f�t�H���g�ň��.
	LPD3DXANIMATIONSET			m_pAnimSet[MAX_ANIM_SET];
};

