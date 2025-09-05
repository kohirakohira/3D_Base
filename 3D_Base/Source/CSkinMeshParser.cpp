/***************************************************************************************************
*	SkinMeshCode Version 2.50
*	LastUpdate	: 2025/06/23.
**/
#include "CSkinMeshParser.h"
#include <stdlib.h>
#include <string.h>
#include <crtdbg.h>

#include <memory>


//�t���[�����쐬����.
HRESULT MY_HIERARCHY::CreateFrame( LPCSTR Name, LPD3DXFRAME *ppNewFrame )
{
	HRESULT hr = S_OK;
	MYFRAME *pFrame;

	*ppNewFrame = nullptr;

	pFrame = new MYFRAME();
	if ( pFrame == nullptr )
	{
		return E_OUTOFMEMORY;
	}
	int length = static_cast<int>( strlen( Name ) ) + 1;
	pFrame->Name = new CHAR[length]();
	if ( !pFrame->Name )
	{
		return E_FAIL;
	}
	strcpy_s( pFrame->Name, sizeof( CHAR )*length, Name );

	D3DXMatrixIdentity( &pFrame->TransformationMatrix );
	D3DXMatrixIdentity( &pFrame->CombinedTransformationMatrix );

	*ppNewFrame = pFrame;

	return S_OK;
}

//���b�V���R���e�i�[���쐬����.
HRESULT MY_HIERARCHY::CreateMeshContainer(
	LPCSTR Name, CONST D3DXMESHDATA* pMeshData,
	CONST D3DXMATERIAL* pMaterials, CONST D3DXEFFECTINSTANCE* pEffectInstances,
	DWORD NumMaterials, CONST DWORD *pAdjacency, LPD3DXSKININFO pSkinInfo,
	LPD3DXMESHCONTAINER *ppMeshContainer )
{
	*ppMeshContainer = nullptr;
	MYMESHCONTAINER *pMeshContainer = new MYMESHCONTAINER();

	int length = static_cast<int>( strlen( Name ) ) + 1;
	pMeshContainer->Name = new CHAR[length]();
	strcpy_s( pMeshContainer->Name, sizeof( CHAR ) * length, Name );

	memcpy( &pMeshContainer->MeshData, pMeshData, sizeof( pMeshContainer->MeshData ) );
	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

	int NumPoly = pMeshContainer->MeshData.pMesh->GetNumFaces();
	//���b�V���𕡐�����.
	//	�Ȃ����X�L�����b�V�����ƁA���̊֐��𔲂��������D3DX�����Ń��b�V���|�C���^�[�����������Ȃ��Ă��܂��̂�.
	LPDIRECT3DDEVICE9 pDevice = nullptr;
	pMeshContainer->MeshData.pMesh->GetDevice( &pDevice );

	LPD3DXMESH pMesh = nullptr;
	pMeshContainer->MeshData.pMesh->CloneMesh( 0, nullptr, pDevice, &pMesh );
	//SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
	pMeshContainer->MeshData.pMesh = pMesh;

	//���b�V���̃}�e���A���ݒ�.
	pMeshContainer->NumMaterials = max( 1, NumMaterials );
	pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials]();
	pMeshContainer->pAdjacency = new DWORD[pMeshContainer->MeshData.pMesh->GetNumFaces() * 3]();
	if ( ( pMeshContainer->pAdjacency == nullptr ) || ( pMeshContainer->pMaterials == nullptr ) )
	{
		return E_FAIL;
	}
	memcpy( pMeshContainer->pAdjacency, pAdjacency, sizeof( DWORD ) * NumPoly * 3 );

	if ( NumMaterials > 0 )
	{
		for( DWORD i = 0; i < NumMaterials; i++ )
		{
			memcpy( &pMeshContainer->pMaterials[i], &pMaterials[i], sizeof( D3DXMATERIAL ) );
			if ( pMaterials[i].pTextureFilename != nullptr )
			{
				int length = 512;
				pMeshContainer->pMaterials[i].pTextureFilename = new CHAR[length]();
				strcpy_s( pMeshContainer->pMaterials[i].pTextureFilename,
					sizeof( CHAR ) * length, pMaterials[i].pTextureFilename );
			}
		}
	}
	//�}�e���A�����Ȃ������ꍇ.
	else
	{
		pMeshContainer->pMaterials[0].pTextureFilename = nullptr;
		memset( &pMeshContainer->pMaterials[0].MatD3D, 0, sizeof( D3DMATERIAL9 ) );
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
	}
	//���Y���b�V�����X�L�����������Ă���ꍇ(�X�L�����b�V���ŗL�̏���).
	if ( pSkinInfo != nullptr )
	{
		DWORD BoneNum = 0;

		pMeshContainer->pSkinInfo = pSkinInfo;
		pSkinInfo->AddRef();
		BoneNum = pSkinInfo->GetNumBones();
		pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[BoneNum]();

		for( DWORD i = 0; i < BoneNum; i++ )
		{
			memcpy( &pMeshContainer->pBoneOffsetMatrices[i],
				pMeshContainer->pSkinInfo->GetBoneOffsetMatrix( i ), sizeof( D3DMATRIX ) );
		}
	}
	//���[�J���ɐ����������b�V���R���e�i�[���Ăяo�����ɃR�s�[����.
	*ppMeshContainer = pMeshContainer;

	return S_OK;
}

//�t���[����j������.
HRESULT MY_HIERARCHY::DestroyFrame( LPD3DXFRAME pFrameToFree )
{
	if ( pFrameToFree->Name ) {
		delete[] pFrameToFree->Name;
		pFrameToFree->Name = nullptr;
	}
	if ( pFrameToFree->pMeshContainer ) {
		DestroyMeshContainer( pFrameToFree->pMeshContainer );
	}
	if ( pFrameToFree->pFrameFirstChild ) {
		DestroyFrame( pFrameToFree->pFrameFirstChild );
	}
	if ( pFrameToFree->pFrameSibling ) {
		DestroyFrame( pFrameToFree->pFrameSibling );
	}
	delete pFrameToFree;

	return S_OK;
}

//���b�V���R���e�i�[��j������.
HRESULT MY_HIERARCHY::DestroyMeshContainer( LPD3DXMESHCONTAINER pMeshContainerBase )
{
	DWORD dwMaterial;
	MYMESHCONTAINER *pMeshContainer = reinterpret_cast<MYMESHCONTAINER*>( pMeshContainerBase );

	SAFE_DELETE_ARRAY( pMeshContainer->Name );
	SAFE_RELEASE( pMeshContainer->pSkinInfo );
	SAFE_DELETE_ARRAY( pMeshContainer->pAdjacency );
	SAFE_DELETE_ARRAY( pMeshContainer->pMaterials );

	if( pMeshContainer->ppBoneMatrix != nullptr ) 
	{
		for( DWORD i = 0; i < pMeshContainer->BoneNum; i++ ) {
			pMeshContainer->ppBoneMatrix[i] = nullptr;
		}
		SAFE_DELETE_ARRAY( pMeshContainer->ppBoneMatrix );
	}

	if( pMeshContainer->ppTextures != nullptr )
	{
		for( dwMaterial = 0; dwMaterial < pMeshContainer->NumMaterials; dwMaterial++ )
		{
			SAFE_RELEASE( pMeshContainer->ppTextures[dwMaterial] );
		}
	}
	SAFE_DELETE_ARRAY( pMeshContainer->ppTextures );

	SAFE_RELEASE( pMeshContainer->MeshData.pMesh );

	if ( pMeshContainer->pBoneBuffer != nullptr )
	{
		SAFE_RELEASE( pMeshContainer->pBoneBuffer );
		SAFE_DELETE_ARRAY( pMeshContainer->pBoneOffsetMatrices );
	}

	SAFE_DELETE( pMeshContainer );

	return S_OK;
}


/*******************************************************************************************************************************************
*
*	�ȍ~�A�p�[�T�[�N���X.
*
**/
//�R���X�g���N�^.
D3DXPARSER::D3DXPARSER()
	: cHierarchy		()
	, m_pHierarchy		( nullptr )
	, m_pFrameRoot		( nullptr )
	, m_pAnimController	( nullptr )
	, m_pAnimSet		()
{
#if _DEBUG
	_CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF );
#endif
}

//�f�X�g���N�^.
D3DXPARSER::~D3DXPARSER()
{
}

//�{�[���s��̗̈�m��.
HRESULT D3DXPARSER::AllocateBoneMatrix( LPD3DXMESHCONTAINER pMeshContainerBase )
{
	MYFRAME *pFrame = nullptr;
	DWORD dwBoneNum = 0;

	MYMESHCONTAINER *pMeshContainer = reinterpret_cast<MYMESHCONTAINER*>( pMeshContainerBase );
	if ( pMeshContainer->pSkinInfo == nullptr )
	{
		return S_OK;
	}
	dwBoneNum = pMeshContainer->pSkinInfo->GetNumBones();
	pMeshContainer->ppBoneMatrix = new D3DXMATRIX*[dwBoneNum]();

	for( DWORD i = 0; i < dwBoneNum; i++ )
	{
		//�܂���nullptr�ŏ�����.
		pMeshContainer->ppBoneMatrix[i] = nullptr;

		pFrame = reinterpret_cast<MYFRAME*>(
			D3DXFrameFind( m_pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName( i ) ) );
		if ( pFrame == nullptr )
		{
			return E_FAIL;
		}
		pMeshContainer->ppBoneMatrix[i] = &pFrame->CombinedTransformationMatrix;

	}
	return S_OK;
}

//�S�Ẵ{�[���s��̗̈���m��(�ċA�֐�).
HRESULT D3DXPARSER::AllocateAllBoneMatrices( LPD3DXFRAME pFrame )
{
	if ( pFrame->pMeshContainer != nullptr )
	{
		if ( FAILED( AllocateBoneMatrix( pFrame->pMeshContainer )))
		{
			return E_FAIL;
		}
	}
	if ( pFrame->pFrameSibling != nullptr )
	{
		if ( FAILED( AllocateAllBoneMatrices( pFrame->pFrameSibling )))
		{
			return E_FAIL;
		}
	}
	if ( pFrame->pFrameFirstChild != nullptr )
	{
		if ( FAILED( AllocateAllBoneMatrices( pFrame->pFrameFirstChild )))
		{
			return E_FAIL;
		}
	}
	return S_OK;
}

//X�t�@�C�����烁�b�V����ǂݍ���.
HRESULT D3DXPARSER::LoadMeshFromX( LPDIRECT3DDEVICE9 pDevice9, LPCTSTR fileName )
{
	//X�t�@�C������A�j���[�V�������b�V����ǂݍ��ݍ쐬����
	m_pHierarchy = new MY_HIERARCHY();
	if ( FAILED(
		D3DXLoadMeshHierarchyFromX(
			fileName, D3DXMESH_MANAGED, pDevice9, m_pHierarchy,
			nullptr, &m_pFrameRoot, &m_pAnimController )))
	{
		_ASSERT_EXPR( false, L"�t�@�C���̓ǂݍ��݂Ɏ��s���܂���" );
		return E_FAIL;
	}
	//�{�[�����������肠��.
	AllocateAllBoneMatrices( m_pFrameRoot );

	/*
		m_pContainer = (MYMESHCONTAINER*)GetMeshContainer( m_pFrameRoot );

		if( m_pContainer == nullptr ){
			_ASSERT_EXPR( false, L"���b�V���R���e�i��������Ȃ�����" );
			return E_FAIL;
		}
	*/

	UINT AnimMax = m_pAnimController->GetNumAnimationSets();
	//�A�j���[�V�����Z�b�g�𓾂�.
	for( UINT i = 0; i < AnimMax; i++ )
	{
		m_pAnimController->GetAnimationSet( i, &m_pAnimSet[i] );
	}

	return S_OK;
}

//�t���[�����̃��b�V�����Ƀ��[���h�ϊ��s����X�V����.
VOID D3DXPARSER::UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix )
{
	MYFRAME *pFrame = reinterpret_cast<MYFRAME*>( pFrameBase );

	if ( pParentMatrix != nullptr )
	{
		D3DXMatrixMultiply( &pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix );
	}
	else
	{
		pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;
	}
	if ( pFrame->pFrameSibling != nullptr )
	{
		UpdateFrameMatrices( pFrame->pFrameSibling, pParentMatrix );
	}
	if ( pFrame->pFrameFirstChild != nullptr )
	{
		UpdateFrameMatrices( pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix );
	}
}

//���_���̎擾.
int D3DXPARSER::GetNumVertices( MYMESHCONTAINER* pContainer )
{
	return pContainer->MeshData.pMesh->GetNumVertices();
}

//�ʐ��̎擾.
int D3DXPARSER::GetNumFaces( MYMESHCONTAINER* pContainer )
{
	return pContainer->MeshData.pMesh->GetNumFaces();
}

//�}�e���A�����̎擾.
int D3DXPARSER::GetNumMaterials( MYMESHCONTAINER* pContainer )
{
	return pContainer->NumMaterials;
}

//UV�̎擾.
int D3DXPARSER::GetNumUVs( MYMESHCONTAINER* pContainer )
{
	//���_���Ɠ����������ݒ肳��Ă�͂�.
	return pContainer->MeshData.pMesh->GetNumVertices();
}

//�w�肳�ꂽ�{�[�����e�����y�ڂ����_����Ԃ�.
int D3DXPARSER::GetNumBoneVertices( MYMESHCONTAINER* pContainer, int BoneIndex )
{
	return pContainer->pSkinInfo->GetNumBoneInfluences( BoneIndex );
}

//�w�肳�ꂽ�{�[�����e�����y�ڂ����_�̃C���f�b�N�X��Ԃ� ��2�����́A�e�����󂯂钸�_�̃C���f�b�N�X�O���[�v���̃C���f�b�N�X�i�C���f�b�N�X���Ⴂ���j
//�Ⴆ�΃{�[���ɉe���������钸�_���S���Ƃ��āA���̃{�[���ɉe����������S�̒��_�̂���2�Ԗڂ̃C���f�b�N�X��m�肽���ꍇ�́AIndexInGroup��1���w�肷��i0�X�^�[�g�Ȃ̂Łj
DWORD D3DXPARSER::GetBoneVerticesIndices(MYMESHCONTAINER* pContainer, int BoneIndex, int IndexInGroup)
{
	auto Num = pContainer->pSkinInfo->GetNumBoneInfluences( BoneIndex );
	auto pVerts = std::make_unique<DWORD[]>(Num);
	auto pWights = std::make_unique<float[]>(Num);

	DWORD dwRslt = 0;

	if ( FAILED(
		pContainer->pSkinInfo->GetBoneInfluence( BoneIndex, pVerts.get(), pWights.get() )))
	{
		MessageBox( nullptr, _T( "�{�[���e�����󂯂钸�_������Ȃ�" ), _T( "error" ), MB_OK );
	}
	else {
		dwRslt = pVerts[IndexInGroup];
	}

	return dwRslt;
}

//�w�肳�ꂽ�{�[�����e�����y�ڂ����_�̃{�[���E�F�C�g��Ԃ� ��2�����́A�e�����󂯂钸�_�̃C���f�b�N�X�O���[�v���̃C���f�b�N�X�i�C���f�b�N�X���Ⴂ���j
//�Ⴆ�΃{�[���ɉe���������钸�_���S���Ƃ��āA���̃{�[���ɉe����������S�̒��_�̂���2�Ԗڂ̒��_�̃{�[���E�F�C�g��m�肽���ꍇ�́AIndexInGroup��1���w�肷��i0�X�^�[�g�Ȃ̂Łj
double D3DXPARSER::GetBoneVerticesWeights( MYMESHCONTAINER* pContainer, int BoneIndex, int IndexInGroup )
{
	auto Num = pContainer->pSkinInfo->GetNumBoneInfluences( BoneIndex );
	auto pVerts = std::make_unique<DWORD[]>(Num);
	auto pWights = std::make_unique<float[]>(Num);
	double dRslt = 0.0f;

	if (FAILED(
		pContainer->pSkinInfo->GetBoneInfluence( BoneIndex, pVerts.get(), pWights.get() )))
	{
		MessageBox( nullptr, _T( "�{�[���e�����󂯂钸�_������Ȃ�" ), _T( "error" ), MB_OK );
	}
	else
	{
		dRslt = static_cast<double>( pWights[IndexInGroup] );
	}

	return dRslt;
}

//���_���W�̎擾.
D3DXVECTOR3 D3DXPARSER::GetVertexCoord( MYMESHCONTAINER* pContainer, DWORD Index )
{
	LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;
	pContainer->MeshData.pMesh->GetVertexBuffer( &pVB );
	DWORD Stride = pContainer->MeshData.pMesh->GetNumBytesPerVertex();
	BYTE *pVertices = nullptr;
	D3DXVECTOR3 Coord;

	if ( SUCCEEDED(
		pVB->Lock( 0, 0, reinterpret_cast<VOID**>( &pVertices ), 0 )))
	{
		pVertices += Index * Stride;
		Coord = *( reinterpret_cast<D3DXVECTOR3*>( pVertices ) );
		pVB->Unlock();
	}
	SAFE_RELEASE( pVB );

	return Coord;
}

//�@���̎擾.
D3DXVECTOR3 D3DXPARSER::GetNormal( MYMESHCONTAINER* pContainer, DWORD Index )
{
	LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;
	pContainer->MeshData.pMesh->GetVertexBuffer( &pVB );
	DWORD Stride = pContainer->MeshData.pMesh->GetNumBytesPerVertex();
	BYTE *pVertices = nullptr;
	D3DXVECTOR3 Normal;

	if ( SUCCEEDED(
		pVB->Lock( 0, 0, reinterpret_cast<VOID**>( &pVertices ), 0)))
	{
		pVertices += Index * Stride;
		pVertices += sizeof( D3DXVECTOR3 );	//���W���i�߂�.
		Normal = *( reinterpret_cast<D3DXVECTOR3*>( pVertices ) );
		pVB->Unlock();
	}
	SAFE_RELEASE( pVB );

	return Normal;
}

//�e�N�X�`���[���W.
D3DXVECTOR2 D3DXPARSER::GetUV( MYMESHCONTAINER* pContainer, DWORD Index )
{
	LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;
	pContainer->MeshData.pMesh->GetVertexBuffer( &pVB );
	DWORD Stride = pContainer->MeshData.pMesh->GetNumBytesPerVertex();
	BYTE *pVertices = nullptr;
	D3DXVECTOR2 UV;

	if ( SUCCEEDED(
		pVB->Lock( 0, 0, reinterpret_cast<VOID**>( &pVertices ), 0 )))
	{
		pVertices += Index * Stride;
		pVertices += sizeof( D3DXVECTOR3 );	//���W���i�߂�.
		pVertices += sizeof( D3DXVECTOR3 );	//�@�����i�߂�.
		UV = *( reinterpret_cast<D3DXVECTOR2*>( pVertices ) );
		pVB->Unlock();
	}
	SAFE_RELEASE( pVB );

	return UV;
}

//�C���f�b�N�X�o�b�t�@���̎w�肵���ʒu�i�C���f�b�N�X�C���f�b�N�X�j�ɂ��钸�_�C���f�b�N�X��Ԃ�.
int D3DXPARSER::GetIndex( MYMESHCONTAINER* pContainer, DWORD Index )
{
	WORD VertIndex = 0;
	WORD* pIndex = nullptr;
	pContainer->MeshData.pMesh->LockIndexBuffer( D3DLOCK_READONLY, reinterpret_cast<VOID**>( &pIndex ) );

	VertIndex = pIndex[Index];

	pContainer->MeshData.pMesh->UnlockIndexBuffer();

	return VertIndex;
}

//�A���r�G���g�i�����j�̎擾.
D3DXVECTOR4 D3DXPARSER::GetAmbient( MYMESHCONTAINER* pContainer, int Index )
{
	D3DXCOLOR color = pContainer->pMaterials[Index].MatD3D.Ambient;
	return D3DXVECTOR4( color.r, color.g, color.b, color.a );
}

//�f�B�t���[�Y�i�g�U���ˌ��j�̎擾.
D3DXVECTOR4 D3DXPARSER::GetDiffuse( MYMESHCONTAINER* pContainer, int Index )
{
	D3DXCOLOR color = pContainer->pMaterials[Index].MatD3D.Diffuse;
	return D3DXVECTOR4( color.r, color.g, color.b, color.a );
}

//�X�y�L�����i���ʔ��ˌ��j�̎擾.
D3DXVECTOR4 D3DXPARSER::GetSpecular( MYMESHCONTAINER* pContainer, int Index )
{
	D3DXCOLOR color = pContainer->pMaterials[Index].MatD3D.Specular;
	return D3DXVECTOR4( color.r, color.g, color.b, color.a );
}

//�G�~�b�V�u�i���Ȕ����j�̎擾.
D3DXVECTOR4 D3DXPARSER::GetEmissive( MYMESHCONTAINER* pContainer, int Index )
{
	D3DXCOLOR color = pContainer->pMaterials[Index].MatD3D.Emissive;
	return D3DXVECTOR4( color.r, color.g, color.b, color.a );
}

//�e�N�X�`���̃p�X�̎擾.
LPSTR D3DXPARSER::GetTexturePath( MYMESHCONTAINER* pContainer, int Index )
{
	return pContainer->pMaterials[Index].pTextureFilename;
}

//�X�y�L�����p���[�̎擾.
float D3DXPARSER::GetSpecularPower( MYMESHCONTAINER* pContainer, int Index )
{
	return pContainer->pMaterials[Index].MatD3D.Power;
}

//���̃|���S�����A�ǂ̃}�e���A���ł��邩��Ԃ�.
int D3DXPARSER::GeFaceMaterialIndex( MYMESHCONTAINER* pContainer, int FaceIndex )
{
	int MaterialIndex = 0;
	DWORD* pBuf = nullptr;
	if ( SUCCEEDED(
		pContainer->MeshData.pMesh->LockAttributeBuffer( D3DLOCK_READONLY, &pBuf )))
	{
		MaterialIndex = pBuf[FaceIndex];
	}
	else
	{
		MessageBox( nullptr, _T( "�����o�b�t�@�̃��b�N���s" ), _T( "error" ), MB_OK );
	}
	return MaterialIndex;
}


//FaceIndex�Ԗڂ̃|���S�����`������3���_�̒��ŁAIndexInFace�Ԗ�[0,2]�̒��_�̃C���f�b�N�X��Ԃ�.
int D3DXPARSER::GetFaceVertexIndex( MYMESHCONTAINER* pContainer, int FaceIndex, int IndexInFace )
{
	//�C���f�b�N�X�o�b�t�@�[�𒲂ׂ�Ε�����.
	WORD VertIndex = 0;
	WORD* pIndex = nullptr;
	pContainer->MeshData.pMesh->LockIndexBuffer( D3DLOCK_READONLY, reinterpret_cast<VOID**>( &pIndex ) );

	VertIndex = pIndex[FaceIndex * 3 + IndexInFace];

	pContainer->MeshData.pMesh->UnlockIndexBuffer();

	return VertIndex;
}


D3DXMATRIX D3DXPARSER::GetBindPose( MYMESHCONTAINER* pContainer, int BoneIndex )
{
	return *pContainer->pSkinInfo->GetBoneOffsetMatrix( BoneIndex );
}

//���̊֐�����Ԃ��Ƃ�Update����Ɩ��ʂȂ̂ŁARender�֐����s����Update������̂Ƃ��Ă��̂܂ܓn��.
//�e�N���X�����̃N���X��UpdateMatrices�����s���Ȃ��ꍇ�́AUpdateMatrices���Ȃ��ƍs�񂪍ŐV�ɂȂ�Ȃ��̂ŗ���.
D3DXMATRIX D3DXPARSER::GetNewPose( MYMESHCONTAINER* pContainer, int BoneIndex )
{
	MYMESHCONTAINER* pMyContaiber = reinterpret_cast<MYMESHCONTAINER*>( pContainer );
	return *pMyContaiber->ppBoneMatrix[BoneIndex];
}


LPCSTR D3DXPARSER::GetBoneName( MYMESHCONTAINER* pContainer, int BoneIndex )
{
	return pContainer->pSkinInfo->GetBoneName( BoneIndex );
}


int D3DXPARSER::GetNumBones( MYMESHCONTAINER* pContainer )
{
	return pContainer->pSkinInfo->GetNumBones();
}

//���b�V���R���e�i���Ăяo��(�ċA����)
LPD3DXMESHCONTAINER D3DXPARSER::GetMeshContainer( LPD3DXFRAME pFrame )
{
	LPD3DXMESHCONTAINER pCon = nullptr;

	//���b�V���R���e�i����ΕԂ�.
	if ( pFrame->pMeshContainer ) {
		return pFrame->pMeshContainer;
	}

	//����������A�q�̃t���[�����`�F�b�N.
	//���������q�t���[������H
	if ( pFrame->pFrameFirstChild ) {
		//����΃`�F�b�N����.
		pCon = GetMeshContainer( pFrame->pFrameFirstChild );
	}

	//�q�̃t���[���ŉ��w�܂Ń`�F�b�N������������Ȃ�����.
	if ( pCon == nullptr ) {
		//�Z��̃t���[�����T��.
		//���������Z��t���[������H
		if ( pFrame->pFrameSibling ) {
			//�����΃`�F�b�N����.
			pCon = GetMeshContainer( pFrame->pFrameSibling );
		}
	}

	//������Ȃ��ꍇ��nullptr������.
	return pCon;
}

//�A�j���[�V�����Z�b�g�̐؂�ւ�.
//	��Q�����́A�ʂɐ��䂷��p��AnimationController���Z�b�g����.
//	�@�s�v�ȏꍇ�́Anullptr��ݒ肷�邱��.
void D3DXPARSER::ChangeAnimSet( int Index, LPD3DXANIMATIONCONTROLLER pAC )
{
	D3DXTRACK_DESC TrackDesc;		//�A�j���[�V�����g���b�N�\����.

	//���ȉ�3�́A�قڌŒ��OK.
	TrackDesc.Weight	= 1.0f;	//�d��.
	TrackDesc.Speed		= 1.0f;	//����.
	TrackDesc.Enable	= TRUE;	//�L��.

	TrackDesc.Priority = D3DXPRIORITY_LOW;

	TrackDesc.Position = 0.0;		//�t���[���ʒu(�J�n�ʒu���w��ł���)

	LPD3DXANIMATIONCONTROLLER pTmpAC;
	pTmpAC = pAC ? pAC : m_pAnimController;

	//�w��(Index�j�̃A�j���[�V�����g���b�N�ɕύX.
	pTmpAC->SetTrackDesc( 0, &TrackDesc );
	pTmpAC->SetTrackAnimationSet( 0, m_pAnimSet[Index] );
	pTmpAC->SetTrackEnable( Index, TRUE );
}

//�A�j���[�V�����Z�b�g�̐؂�ւ�(�J�n�t���[���w��\��)
//�@��Q�����ɊJ�n�������t���[�����w�肷��.
//	�@���S��~�������ꍇ�́A�O��ŃA�j���[�V�������x��0.0f�Ɏw�肵�Ă��K�v������.
//	��R�����́A�ʂɐ��䂷��p��AnimationController���Z�b�g����.
//	�@�s�v�ȏꍇ�́Anullptr��ݒ肷�邱��.
void D3DXPARSER::ChangeAnimSet_StartPos(
	int Index, double StartFramePos, LPD3DXANIMATIONCONTROLLER pAC )
{
	D3DXTRACK_DESC TrackDesc;		//�A�j���[�V�����g���b�N�\����.

	//���ȉ�3�́A�قڌŒ��OK.
	TrackDesc.Weight = 1.0f;	//�d��.
	TrackDesc.Speed	 = 1.0f;	//����.
	TrackDesc.Enable = TRUE;	//�L��.

	TrackDesc.Priority = D3DXPRIORITY_LOW;
	TrackDesc.Position = StartFramePos;

	LPD3DXANIMATIONCONTROLLER pTmpAC;
	pTmpAC = ( pAC != nullptr ) ? pAC : m_pAnimController;

	//�w��(Index�j�̃A�j���[�V�����g���b�N�ɕύX.
	pTmpAC->SetTrackDesc( 0, &TrackDesc );
	pTmpAC->SetTrackAnimationSet( 0, m_pAnimSet[Index] );
	pTmpAC->SetTrackEnable( Index, TRUE );
}

//�A�j���[�V������~���Ԃ��擾.
double D3DXPARSER::GetAnimPeriod( int Index)
{
	if( Index < 0 || MAX_ANIM_SET <= Index ) {
		return 0.0;
	}
	return m_pAnimSet[Index]->GetPeriod();
}

//�ő�A�j���[�V���������擾.
int D3DXPARSER::GetAnimMax( LPD3DXANIMATIONCONTROLLER pAC )
{
	if( pAC != nullptr ) {
		return pAC->GetNumAnimationSets();
	}
	return m_pAnimController->GetNumAnimationSets();
}

//�w�肵���{�[�����(�s��)���擾����֐�.
bool D3DXPARSER::GetMatrixFromBone( LPCSTR BoneName, D3DXMATRIX* pOutMat )
{
	LPD3DXFRAME pFrame;
	pFrame = reinterpret_cast<MYFRAME*>( D3DXFrameFind( m_pFrameRoot, BoneName ) );

	if ( pFrame == nullptr ) {
		return false;
	}

	MYFRAME* pMyFrame = reinterpret_cast<MYFRAME*>( pFrame );
	//�ʒu�����擾.
	*pOutMat = pMyFrame->CombinedTransformationMatrix;

	return true;
}

//�w�肵���{�[�����(���W)���擾����֐�.
bool D3DXPARSER::GetPosFromBone( LPCSTR BoneName, D3DXVECTOR3* pOutPos )
{
	D3DXMATRIX mBone;
	if ( GetMatrixFromBone( BoneName, &mBone ) == false ) {
		return false;
	}
	pOutPos->x = mBone._41;
	pOutPos->y = mBone._42;
	pOutPos->z = mBone._43;

	return true;
}

//�ꊇ�������.
HRESULT D3DXPARSER::Release()
{
	//�쐬�������̂��Ō�ɍ�������̂��珇������Ă���.
	//��{�I�ɂ� new �������̂� delete ���Ă���.

	//�A�j���[�V�����Z�b�g�̉��.
	DWORD AnimMax = m_pAnimController->GetNumAnimationSets();
	for( DWORD i = 0; i < AnimMax; i++ )
	{
		SAFE_RELEASE( m_pAnimSet[i] );
	}

	//�A�j���[�V�����R���g���[���폜.
	SAFE_RELEASE( m_pAnimController );

	//�t���[���ƃ��b�V���R���e�i�̍폜.
	m_pHierarchy->DestroyFrame( m_pFrameRoot );

	//�����������H
	if ( m_pFrameRoot != nullptr ) {
		m_pFrameRoot = nullptr;
	}

	//Hierarchy�폜.
	if ( m_pHierarchy != nullptr ) {
		delete m_pHierarchy;
		m_pHierarchy = nullptr;
	}

	return S_OK;
}
