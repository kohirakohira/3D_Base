/***************************************************************************************************
*	SkinMeshCode Version 2.50
*	LastUpdate	: 2025/06/23.
**/
#include "CSkinMesh.h"
#include "CDirectX9.h"
#include "CDirectX11.h"

#include <stdlib.h>	//�}���`�o�C�g������Unicode�����ϊ��ŕK�v.
#include <locale.h>

#include <crtdbg.h>

#include <memory>
#include <algorithm>

//����ւ��֐�.
template <typename T>
void swap( T& a, T& b )
{
	T temp = a;
	a = b;
	b = temp;
}
//�͈͓��ɔ[�߂�֐�.
template <typename T>
const T& clamp( const T& val, const T& low, const T& high )
{
	return val > high ? high : val < low ? low : val;
}

//�V�F�[�_��(�f�B���N�g�����܂�)
const TCHAR SHADER_NAME[] = _T( "Data\\Shader\\SkinMesh.hlsl" );

//�R���X�^���g�o�b�t�@��ݒ肷��X���b�g�ԍ�.
enum enCBSlot
{
	Mesh,		//���b�V��.
	Material,	//�}�e���A��.
	Frame,		//�t���[��.
	Bones,		//�{�[��.
};

/******************************************************************************************************************************************
*
*	�ȍ~�A�X�L�����b�V���N���X.
*
**/
//�R���X�g���N�^.
CSkinMesh::CSkinMesh()
	: m_pDx9				( nullptr )
	, m_pDevice9			( nullptr )

	, m_pDx11				( nullptr )
	, m_pDevice11			( nullptr )
	, m_pContext11			( nullptr )

	, m_pVertexShader		( nullptr )
	, m_pVertexLayout		( nullptr )
	, m_pPixelShader		( nullptr )
	, m_pSampleLinear		( nullptr )

	, m_pCBufferPerMesh		( nullptr )
	, m_pCBufferPerMaterial	( nullptr )
	, m_pCBufferPerFrame	( nullptr )
	, m_pCBufferPerBone		( nullptr )

	, m_Position			()
	, m_Rotation			()
	, m_Scale				( D3DXVECTOR3( 1.0f, 1.0f, 1.0f) )

	, m_mWorld				()
	, m_mRotation			()

	, m_mView				()
	, m_mProj				()

	, m_Light				()
	, m_CamPos				()

	, m_AnimSpeed			( 0.001 )	//��悸�A���̒l.
	, m_AnimTime			()

//	, m_pReleaseMaterial	( nullptr )
	, m_pD3dxMesh			( nullptr )

	, m_FilePath			()
	, m_Frame				()
{
}


//�f�X�g���N�^.
CSkinMesh::~CSkinMesh()
{
	//�������.
	Release();

	//�V�F�[�_��T���v���֌W.
	SAFE_RELEASE( m_pSampleLinear );
	SAFE_RELEASE( m_pVertexShader );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pVertexLayout );

	//�R���X�^���g�o�b�t�@�֌W.
	SAFE_RELEASE( m_pCBufferPerBone );
	SAFE_RELEASE( m_pCBufferPerFrame );
	SAFE_RELEASE( m_pCBufferPerMaterial );
	SAFE_RELEASE( m_pCBufferPerMesh );

	SAFE_RELEASE( m_pD3dxMesh );

	//Dx9 �f�o�C�X�֌W.
	m_pDevice9	= nullptr;
	m_pDx9		= nullptr;

	//Dx11 �f�o�C�X�֌W.
	m_pContext11= nullptr;
	m_pDevice11	= nullptr;
	m_pDx11		= nullptr;
}


//������.
HRESULT CSkinMesh::Init(
	CDirectX9& pDx9, CDirectX11& pDx11, LPCTSTR FileName )
{
	m_pDx9 = &pDx9;
	m_pDevice9 = m_pDx9->GetDevice();

	m_pDx11 = &pDx11;
	m_pDevice11 = m_pDx11->GetDevice();
	m_pContext11 = m_pDx11->GetContext();

	//���f���ǂݍ���.
	if( FAILED( LoadXMesh( FileName ) ) )
	{
		return E_FAIL;
	}
	//�V�F�[�_�̍쐬.
	if( FAILED( CreateShader() ) )
	{
		return E_FAIL;
	}
	//�R���X�^���g�o�b�t�@(���b�V������).
	if( FAILED(
		CreateCBuffer( &m_pCBufferPerMesh, sizeof( CBUFFER_PER_MESH ) ) ) )
	{
		return E_FAIL;
	}
	//�R���X�^���g�o�b�t�@(���b�V������).
	if( FAILED(
		CreateCBuffer( &m_pCBufferPerMaterial, sizeof( CBUFFER_PER_MATERIAL ) ) ) )
	{
		return E_FAIL;
	}
	//�R���X�^���g�o�b�t�@(���b�V������).
	if( FAILED(
		CreateCBuffer( &m_pCBufferPerFrame, sizeof( CBUFFER_PER_FRAME ) ) ) )
	{
		return E_FAIL;
	}
	//�R���X�^���g�o�b�t�@(���b�V������).
	if( FAILED(
		CreateCBuffer( &m_pCBufferPerBone, sizeof( CBUFFER_PER_BONES ) ) ) )
	{
		return E_FAIL;
	}
	//�e�N�X�`���[�p�T���v���[�쐬.
	if( FAILED( CreateSampler( &m_pSampleLinear ) ) )
	{
		return E_FAIL;
	}

	return S_OK;
}


//�V�F�[�_������.
HRESULT	CSkinMesh::CreateShader()
{
	//D3D11�֘A�̏�����
	ID3DBlob *pCompiledShader = nullptr;
	ID3DBlob *pErrors = nullptr;
	UINT	uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif//#ifdef _DEBUG

	//HLSL����o�[�e�b�N�X�V�F�[�_�̃u���u���쐬.
	if( FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME, nullptr, nullptr,
			"VS_Main", "vs_5_0",
			uCompileFlag, 0, nullptr,
			&pCompiledShader, &pErrors, nullptr ) ) )
	{
		_ASSERT_EXPR( false, L"hlsl�ǂݍ��ݎ��s" );
		return E_FAIL;
	}
	SAFE_RELEASE( pErrors );

	//��L�ō쐬�����u���u����o�[�e�b�N�X�V�F�[�_���쐬.
	if( FAILED(
		m_pDevice11->CreateVertexShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr, &m_pVertexShader ) ) )
	{
		SAFE_RELEASE( pCompiledShader );
		_ASSERT_EXPR( false, L"�o�[�e�b�N�X�V�F�[�_�쐬���s" );
		return E_FAIL;
	}
	//���_�C���v�b�g���C�A�E�g���`	
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,							  0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_INDEX",	0, DXGI_FORMAT_R32G32B32A32_UINT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_WEIGHT",0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof( layout ) / sizeof( layout[0] );

	//���_�C���v�b�g���C�A�E�g���쐬
	if( FAILED(
		m_pDevice11->CreateInputLayout(
			layout, numElements, pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(), &m_pVertexLayout ) ) )
	{
		_ASSERT_EXPR( false, L"���_�C���v�b�g���C�A�E�g�쐬���s" );
		return E_FAIL;
	}
	//���_�C���v�b�g���C�A�E�g���Z�b�g
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	//HLSL����s�N�Z���V�F�[�_�̃u���u���쐬.
	if(FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME, nullptr, nullptr,
			"PS_Main", "ps_5_0",
			uCompileFlag, 0, nullptr,
			&pCompiledShader, &pErrors, nullptr ) ) )
	{
		_ASSERT_EXPR( false, L"hlsl�ǂݍ��ݎ��s" );
		return E_FAIL;
	}
	SAFE_RELEASE( pErrors );
	//��L�ō쐬�����u���u����s�N�Z���V�F�[�_���쐬.
	if( FAILED(
		m_pDevice11->CreatePixelShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr, &m_pPixelShader ) ) )
	{
		SAFE_RELEASE( pCompiledShader );
		_ASSERT_EXPR( false, L"�s�N�Z���V�F�[�_�쐬���s" );
		return E_FAIL;
	}
	SAFE_RELEASE( pCompiledShader );

	return S_OK;
}

//X�t�@�C������X�L���֘A�̏���ǂݏo���֐�.
HRESULT CSkinMesh::ReadSkinInfo(
	MYMESHCONTAINER* pContainer, VERTEX* pVB, SKIN_PARTS_MESH* pParts )
{
	//X�t�@�C�����璊�o���ׂ����́A
	//�u���_���Ƃ̃{�[���C���f�b�N�X�v�u���_���Ƃ̃{�[���E�F�C�g�v.
	//�u�o�C���h�s��v�u�|�[�Y�s��v��4����.

	int i, k, m, n;			//�e�탋�[�v�ϐ�.
	int NumVertex	= 0;	//���_��.
	int NumBone		= 0;	//�{�[����.

	//���_��.
	NumVertex	= m_pD3dxMesh->GetNumVertices( pContainer );
	//�{�[����.
	NumBone		= m_pD3dxMesh->GetNumBones( pContainer );

	//���ꂼ��̃{�[���ɉe�����󂯂钸�_�𒲂ׂ�.
	//��������t�ɁA���_�x�[�X�Ń{�[���C���f�b�N�X�E�d�݂𐮓ڂ���.
	for( i = 0; i < NumBone; i++ )
	{
		//���̃{�[���ɉe�����󂯂钸�_��.
		auto NumIndex = m_pD3dxMesh->GetNumBoneVertices( pContainer, i );

		auto pIndex	 = std::make_unique<int[]>( NumIndex );
		auto pWeight = std::make_unique<double[]>( NumIndex );

		for( k = 0; k < NumIndex; k++ )
		{
			pIndex[k]	= m_pD3dxMesh->GetBoneVerticesIndices( pContainer, i, k );
			pWeight[k]	= m_pD3dxMesh->GetBoneVerticesWeights( pContainer, i, k );
		}

		//���_������C���f�b�N�X�����ǂ��āA���_�T�C�h�Ő�������.
		for( k = 0; k < NumIndex; k++ )
		{
			//X�t�@�C����CG�\�t�g���{�[��4�{�ȓ��Ƃ͌���Ȃ�.
			//5�{�ȏ�̏ꍇ�́A�d�݂̑傫������4�{�ɍi��.

			VERTEX* pV = &pVB[pIndex[k]];

			//�E�F�C�g�̑傫�����Ƀ\�[�g(�o�u���\�[�g).
			for( m = 4; m > 1; m-- )
			{
				for( n = 1; n < m; n++ )
				{
					if( pV->BoneWeight[n - 1] < pV->BoneWeight[n] )
					{
						swap( pV->BoneWeight[n - 1], pV->BoneWeight[n] );
						swap( pV->BoneIndex[ n - 1], pV->BoneIndex[ n] );
					}
				}
			}
			//�\�[�g��́A�Ō�̗v�f�Ɉ�ԏ������E�F�C�g�������Ă�͂�.
			bool flag = false;
			for( m = 0; m < 4; m++ )
			{
				if( pV->BoneWeight[m] == 0 )
				{
					flag = true;
					pV->BoneIndex[m] = i;
					pV->BoneWeight[m] = static_cast<float>( pWeight[k] );
					break;
				}
			}
			if( flag == false )
			{
				pV->BoneIndex[3] = i;
				pV->BoneWeight[3] = static_cast<float>( pWeight[k] );
				break;
			}
		}
	}


	//�{�[������.
	pParts->NumBone	= NumBone;
	pParts->pBoneArray	= new BONE[NumBone]();
	//�|�[�Y�s��𓾂�(�����|�[�Y).
	for( i = 0; i < pParts->NumBone; i++ )
	{
		pParts->pBoneArray[i].mBindPose
			= m_pD3dxMesh->GetBindPose( pContainer, i );
	}

	return S_OK;
}

//X����X�L�����b�V�����쐬����.
//	���Ӂj�f�ށiX)�̂ق��́A�O�p�|���S���ɂ��邱��.
HRESULT CSkinMesh::LoadXMesh( LPCTSTR FileName )
{
	//�t�@�C�������p�X���Ǝ擾.
	lstrcpy( m_FilePath, FileName );

	//X�t�@�C���ǂݍ���.
	m_pD3dxMesh = new D3DXPARSER();
	m_pD3dxMesh->LoadMeshFromX( m_pDevice9, FileName );


	//�S�Ẵ��b�V�����쐬����.
	BuildAllMesh( m_pD3dxMesh->m_pFrameRoot );

	return S_OK;
}


//Direct3D�̃C���f�b�N�X�o�b�t�@�[�쐬.
HRESULT CSkinMesh::CreateIndexBuffer(
	DWORD Size, int* pIndex, ID3D11Buffer** ppIndexBuffer )
{
	D3D11_BUFFER_DESC bd;
	bd.Usage			= D3D11_USAGE_DEFAULT;
	bd.ByteWidth		= Size;
	bd.BindFlags		= D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags	= 0;
	bd.MiscFlags		= 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem			= pIndex;
	InitData.SysMemPitch		= 0;
	InitData.SysMemSlicePitch	= 0;

	if( FAILED(
		m_pDevice11->CreateBuffer( &bd, &InitData, ppIndexBuffer ) ) )
	{
		return E_FAIL;
	}
	
	return S_OK;
}


//�����_�����O.
void CSkinMesh::Render(
	const D3DXMATRIX& mView, const D3DXMATRIX& mProj,
	const LIGHT& Light, const D3DXVECTOR3& CamPos,
	LPD3DXANIMATIONCONTROLLER pAC )
{
	m_mView		= mView;
	m_mProj		= mProj;
	m_Light		= Light;
	m_CamPos	= CamPos;

	if ( pAC == nullptr )
	{
		if ( m_pD3dxMesh->m_pAnimController )
		{
			m_pD3dxMesh->m_pAnimController->AdvanceTime( m_AnimSpeed, nullptr );
		}
	}
	else
	{
		pAC->AdvanceTime( m_AnimSpeed, nullptr );
	}

	D3DXMATRIX m;
	D3DXMatrixIdentity( &m );
	m_pD3dxMesh->UpdateFrameMatrices( m_pD3dxMesh->m_pFrameRoot, &m );
	DrawFrame( m_pD3dxMesh->m_pFrameRoot );
}


//�S�Ẵ��b�V�����쐬����.
void CSkinMesh::BuildAllMesh( D3DXFRAME* pFrame )
{
	if( pFrame && pFrame->pMeshContainer )
	{
		CreateAppMeshFromD3DXMesh( pFrame );
	}

	//�ċA�֐�.
	if( pFrame->pFrameSibling != nullptr )
	{
		BuildAllMesh( pFrame->pFrameSibling );
	}
	if( pFrame->pFrameFirstChild != nullptr )
	{
		BuildAllMesh( pFrame->pFrameFirstChild );
	}
}

//���b�V���쐬.
HRESULT CSkinMesh::CreateAppMeshFromD3DXMesh( LPD3DXFRAME p )
{
	MYFRAME* pFrame = reinterpret_cast<MYFRAME*>( p );

//	LPD3DXMESH pD3DXMesh = pFrame->pMeshContainer->MeshData.pMesh;//D3DX���b�V��(��������E�E�E).
	MYMESHCONTAINER* pContainer = reinterpret_cast<MYMESHCONTAINER*>( pFrame->pMeshContainer );

	//�A�v�����b�V��(�E�E�E�����Ƀ��b�V���f�[�^���R�s�[����).
	SKIN_PARTS_MESH* pAppMesh = new SKIN_PARTS_MESH();
	pAppMesh->EnableTexture = false;

	//���O�ɒ��_���A�|���S�������𒲂ׂ�.
	pAppMesh->NumVert	= m_pD3dxMesh->GetNumVertices( pContainer );
	pAppMesh->NumFace	= m_pD3dxMesh->GetNumFaces( pContainer );
	pAppMesh->NumUV		= m_pD3dxMesh->GetNumUVs( pContainer );
	//Direct3D�ł�UV�̐��������_���K�v.
	if( pAppMesh->NumVert < pAppMesh->NumUV ){
		//���L���_���ŁA���_������Ȃ��Ƃ�.
		_ASSERT_EXPR( false,
			L"Direct3D�́AUV�̐��������_���K�v�ł�(UV��u���ꏊ���K�v�ł�)�e�N�X�`���͐������\���Ȃ��Ǝv���܂�" );
		return E_FAIL;
	}
	//�ꎞ�I�ȃ������m��(���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@).
	auto pVB = std::make_unique<VERTEX[]>( pAppMesh->NumVert );
	auto pFaceBuffer = std::make_unique<int[]>( pAppMesh->NumFace * 3 );
	//3���_�|���S���Ȃ̂ŁA1�t�F�C�X=3���_(3�C���f�b�N�X).

	//���_�ǂݍ���.
	for( DWORD i = 0; i < pAppMesh->NumVert; i++ ){
		pVB[i].Position = m_pD3dxMesh->GetVertexCoord( pContainer, i );
	}
	//�|���S�����(���_�C���f�b�N�X)�ǂݍ���.
	for( DWORD i = 0; i < pAppMesh->NumFace*3; i++ ){
		pFaceBuffer[i] = m_pD3dxMesh->GetIndex( pContainer, i );
	}
	//�@���ǂݍ���.
	for( DWORD i = 0; i < pAppMesh->NumVert; i++ ){
		pVB[i].vNormal	= m_pD3dxMesh->GetNormal( pContainer, i);
	}
	//�e�N�X�`�����W�ǂݍ���.
	for( DWORD i = 0; i < pAppMesh->NumVert; i++ ){
		pVB[i].Texture = m_pD3dxMesh->GetUV( pContainer, i );
	}

	//�}�e���A���ǂݍ���.
	pAppMesh->NumMaterial	= m_pD3dxMesh->GetNumMaterials( pContainer );
	pAppMesh->pMaterial		= new MY_SKINMATERIAL[pAppMesh->NumMaterial]();

	//�}�e���A���̐������C���f�b�N�X�o�b�t�@���쐬.
	pAppMesh->ppIndexBuffer = new ID3D11Buffer*[pAppMesh->NumMaterial]();
	//�|���Z�ł͂Ȃ��uID3D11Buffer*�v�̔z��Ƃ����Ӗ�.
	for( DWORD i = 0; i < pAppMesh->NumMaterial; i++ )
	{
		//����(�A���r�G���g).
		pAppMesh->pMaterial[i].Ambient		= m_pD3dxMesh->GetAmbient( pContainer, i );
		//�g�U���ˌ�(�f�B�t���[�Y).
		pAppMesh->pMaterial[i].Diffuse		= m_pD3dxMesh->GetDiffuse( pContainer, i );
		//���ʔ��ˌ�(�X�y�L����).
		pAppMesh->pMaterial[i].Specular		= m_pD3dxMesh->GetSpecular( pContainer, i );
		//���Ȕ���(�G�~�b�V�u).
		pAppMesh->pMaterial[i].Emissive		= m_pD3dxMesh->GetEmissive( pContainer, i );
		//�X�y�L�����p���[.
		pAppMesh->pMaterial[i].SpecularPower= m_pD3dxMesh->GetSpecularPower( pContainer, i );

		//�A���r�G���g��0���ƌ�������̒��ˌ����S��������Ȃ��A�̕����̖��邳���A
		// �^���ÂɂȂ�̂ŁA�Œ�l�ƍō��l��ݒ肷��.
		float low	= 0.3f;	//�K����0.3���炢�ɂ��Ă���.
		float high	= 1.0f;
		pAppMesh->pMaterial[i].Ambient.x = clamp( pAppMesh->pMaterial[i].Ambient.x, low, high );
		pAppMesh->pMaterial[i].Ambient.y = clamp( pAppMesh->pMaterial[i].Ambient.y, low, high );
		pAppMesh->pMaterial[i].Ambient.z = clamp( pAppMesh->pMaterial[i].Ambient.z, low, high );
		//�Ȃ��Ax,y,z(r,g,b)�̂ݑΏ����āAw(a)�͂��̂܂܎g�p����.

#if 0
		//�e�N�X�`��(�f�B�t���[�Y�e�N�X�`���̂�).
#ifdef UNICODE
		WCHAR TexFilename_w[32] = L"";
		//�e�N�X�`�����̃T�C�Y���擾.
		size_t charSize = strlen( m_pD3dxMesh->GetTexturePath( pContainer, i ) ) + 1;
		size_t ret;	//�ϊ����ꂽ������.

		//�}���`�o�C�g�����̃V�[�P���X��Ή����郏�C�h�����̃V�[�P���X�ɕϊ����܂�.
		errno_t err = mbstowcs_s(
			&ret,
			TexFilename_w,
			charSize,
			m_pD3dxMesh->GetTexturePath( pContainer, i ),
			_TRUNCATE );

		LPTSTR name = TexFilename_w;
#else//#ifdef UNICODE
		LPTSTR name = d3dxMaterials[No].pTextureFilename;
#endif//#ifdef UNICODE
#endif

		//�e�N�X�`��(�f�B�t���[�Y�e�N�X�`���̂�).
#ifdef UNICODE
		WCHAR TextureName_w[32] = L"";
		//�����ϊ�.
		LPTSTR name = nullptr;
		LPSTR name_org = m_pD3dxMesh->GetTexturePath(pContainer, i);
		if (name_org != nullptr) {
			ConvertCharaMultiByteToUnicode(TextureName_w, 32, name_org);
			name = TextureName_w;
		}
#else//#ifdef UNICODE
		LPTSTR name = m_pD3dxMesh->GetTexturePath(pContainer, i);
#endif//#ifdef UNICODE

		if( name != nullptr ){
			LPTSTR ret = _tcsrchr( m_FilePath, _T( '\\' ) );
			if( ret != nullptr ){
				int check = static_cast<int>( ret - m_FilePath );
				TCHAR path[512];
				lstrcpy( path, m_FilePath );
				path[check + 1] = '\0';

				//�p�X�̃R�s�[.
				lstrcpy( pAppMesh->pMaterial[i].TextureName, path );
				//�e�N�X�`���t�@�C������A��.
				lstrcat( pAppMesh->pMaterial[i].TextureName, name );
			}
		}
		//�e�N�X�`�����쐬.
		if( pAppMesh->pMaterial[i].TextureName[0] != 0
			&& FAILED(
				D3DX11CreateShaderResourceViewFromFile(
				m_pDevice11, pAppMesh->pMaterial[i].TextureName,
				nullptr, nullptr, &pAppMesh->pMaterial[i].pTexture, nullptr ) ))
		{
			_ASSERT_EXPR( false, L"�e�N�X�`���쐬���s" );
			return E_FAIL;
		}
		//���̃}�e���A���ł���C���f�b�N�X�z����̊J�n�C���f�b�N�X�𒲂ׂ�.
		//����ɃC���f�b�N�X�̌��𒲂ׂ�.
		int count = 0;
		auto pIndex = std::make_unique<int[]>( pAppMesh->NumFace * 3 );
			//�Ƃ肠�����A���b�V�����̃|���S�����Ń������m��.
			//(�����̃|���S���O���[�v�͕K������ȉ��ɂȂ�).

		for( DWORD k = 0; k < pAppMesh->NumFace; k++ )
		{
			//���� i==k �Ԗڂ̃|���S���̃}�e���A���ԍ��Ȃ�.
			if( i == m_pD3dxMesh->GeFaceMaterialIndex( pContainer, k ))
			{
				//k�Ԗڂ̃|���S������钸�_�̃C���f�b�N�X.
				pIndex[count]
					= m_pD3dxMesh->GetFaceVertexIndex( pContainer, k, 0 );	//1��.
				pIndex[count + 1]
					= m_pD3dxMesh->GetFaceVertexIndex( pContainer, k, 1 );	//2��.
				pIndex[count + 2]
					= m_pD3dxMesh->GetFaceVertexIndex( pContainer, k, 2 );	//3��.
				count += 3;
			}
		}
		if( count > 0 ){
			//�C���f�b�N�X�o�b�t�@�쐬.
			CreateIndexBuffer( count * sizeof( int ),
				pIndex.get(), &pAppMesh->ppIndexBuffer[i]);
		}
		else{
			//������̏����ɕs����o������.
			//�J�E���g����0�ȉ��̏ꍇ�́A�C���f�b�N�X�o�b�t�@�� nullptr �ɂ��Ă���.
			pAppMesh->ppIndexBuffer[i] = nullptr;
		}

		//���̃}�e���A�����̃|���S����.
		pAppMesh->pMaterial[i].NumFace = count / 3;

	}

	//�X�L����񂠂�H
	if( pContainer->pSkinInfo == nullptr ){
#ifdef _DEBUG
		//�s���ȃX�L������΂����ŋ�����.�s�v�Ȃ�R�����g�A�E�g���Ă�������.
		TCHAR strDbg[128];
		WCHAR str[64] = L"";
		ConvertCharaMultiByteToUnicode( str, 64, pContainer->Name );
		_stprintf_s( strDbg, _T( "ContainerName:[%s]" ), str );
		MessageBox( nullptr, strDbg, _T( "Not SkinInfo" ), MB_OK );
#endif//#ifdef _DEBUG
		pAppMesh->EnableBones = false;
	}
	else{
		//�X�L�����(�W���C���g�A�E�F�C�g)�ǂݍ���.
		ReadSkinInfo( pContainer, pVB.get(), pAppMesh );
	}

	//�o�[�e�b�N�X�o�b�t�@���쐬.
	D3D11_BUFFER_DESC bd;
	bd.Usage		= D3D11_USAGE_DEFAULT;
	bd.ByteWidth	= sizeof( VERTEX ) * ( pAppMesh->NumVert );
	bd.BindFlags	= D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags= 0;
	bd.MiscFlags	= 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pVB.get();

	HRESULT hRslt = S_OK;
	if( FAILED(
		m_pDevice11->CreateBuffer(
			&bd, &InitData, &pAppMesh->pVertexBuffer ) ) )
	{
		hRslt = E_FAIL;
	}

	//�p�[�c���b�V���ɐݒ�.
	pFrame->pPartsMesh = pAppMesh;

	return hRslt;
}


//�{�[�������̃|�[�Y�ʒu�ɃZ�b�g����֐�.
void CSkinMesh::SetNewPoseMatrices(
	SKIN_PARTS_MESH* pParts, int Frame, MYMESHCONTAINER* pContainer )
{
	//�]�ރt���[����Update���邱��.
	//���Ȃ��ƍs�񂪍X�V����Ȃ�.
	//m_pD3dxMesh->UpdateFrameMatrices(
	// m_pD3dxMesh->m_pFrameRoot)�������_�����O���Ɏ��s���邱��.

	//�܂��A�A�j���[�V�������ԂɌ��������s����X�V����̂�D3DXMESH�ł�
	//�A�j���[�V�����R���g���[����(����)����Ă������̂Ȃ̂ŁA
	//�A�j���[�V�����R���g���[�����g���ăA�j����i�s�����邱�Ƃ��K�v.
	//m_pD3dxMesh->m_pAnimController->AdvanceTime(...)��.
	//�����_�����O���Ɏ��s���邱��.

	if( pParts->NumBone <= 0 ){
		//�{�[���� 0�@�ȉ��̏ꍇ.
	}

	for( int i = 0; i < pParts->NumBone; i++ )
	{
		pParts->pBoneArray[i].mNewPose
			= m_pD3dxMesh->GetNewPose( pContainer, i );
	}
}


//����(���݂�)�|�[�Y�s���Ԃ��֐�.
D3DXMATRIX CSkinMesh::GetCurrentPoseMatrix( SKIN_PARTS_MESH* pParts, int Index )
{
	D3DXMATRIX ret =
		pParts->pBoneArray[Index].mBindPose * pParts->pBoneArray[Index].mNewPose;
	return ret;
}


//�t���[���̕`��.
VOID CSkinMesh::DrawFrame( LPD3DXFRAME p )
{
	MYFRAME*			pFrame	= reinterpret_cast<MYFRAME*>( p );
	SKIN_PARTS_MESH*	pPartsMesh	= pFrame->pPartsMesh;
	MYMESHCONTAINER*	pContainer	= reinterpret_cast<MYMESHCONTAINER*>( pFrame->pMeshContainer );

	if( pPartsMesh != nullptr )
	{
		DrawPartsMesh(
			pPartsMesh, 
			pFrame->CombinedTransformationMatrix,
			pContainer );
	}

	//�ċA�֐�.
	//(�Z��)
	if( pFrame->pFrameSibling != nullptr )
	{
		DrawFrame( pFrame->pFrameSibling );
	}
	//(�e�q)
	if( pFrame->pFrameFirstChild != nullptr )
	{
		DrawFrame( pFrame->pFrameFirstChild );
	}
}
#define SEND_CB_TO_FUNC

//�p�[�c���b�V����`��.
void CSkinMesh::DrawPartsMesh(
	SKIN_PARTS_MESH* pMesh, D3DXMATRIX World, MYMESHCONTAINER* pContainer )
{
	//���[���h�s��Z�o.
	CalcWorldMatrix();

	//�A�j���[�V�����t���[����i�߂� �X�L�����X�V.
	m_Frame++;
	if( m_Frame >= 3600 ){
		m_Frame = 0;
	}
	SetNewPoseMatrices( pMesh, m_Frame, pContainer );

	//�R���X�^���g�o�b�t�@�ɏ��𑗂�(�{�[��).
	SendCBufferPerBone( pMesh );

	//�R���X�^���g�o�b�t�@�ɏ���ݒ�(�t���[������).
	SendCBufferPerFrame();

	//�R���X�^���g�o�b�t�@�ɏ���ݒ�(���b�V������).
	SendCBufferPerMesh();

	//�o�[�e�b�N�X�o�b�t�@���Z�b�g.
	UINT stride = sizeof( VERTEX );
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers(
		0, 1, &pMesh->pVertexBuffer, &stride, &offset );

	//�g�p����V�F�[�_�̃Z�b�g.
	m_pContext11->VSSetShader( m_pVertexShader, nullptr, 0 );
	m_pContext11->PSSetShader( m_pPixelShader, nullptr, 0 );

	//���_�C���v�b�g���C�A�E�g���Z�b�g.
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	//�v���~�e�B�u�E�g�|���W�[���Z�b�g.
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	//�}�e���A���̐������A
	//���ꂼ��̃}�e���A���̃C���f�b�N�X�o�b�t�@��`��.
	for( DWORD i = 0; i < pMesh->NumMaterial; i++ )
	{
		//�g�p����Ă��Ȃ��}�e���A���΍�.
		if( pMesh->pMaterial[i].NumFace == 0 )
		{
			continue;
		}

		//�C���f�b�N�X�o�b�t�@���Z�b�g.
		stride	= sizeof( int );
		offset	= 0;
		m_pContext11->IASetIndexBuffer(
			pMesh->ppIndexBuffer[i],
			DXGI_FORMAT_R32_UINT, 0 );

		//�R���X�^���g�o�b�t�@�ɏ���ݒ�i�}�e���A�����Ɓj
		SendCBufferPerMaterial( &pMesh->pMaterial[i]);

		//�e�N�X�`�����V�F�[�_�ɓn��.
		SendTexture( &pMesh->pMaterial[i]);

		//�`��.
		m_pContext11->DrawIndexed( pMesh->pMaterial[i].NumFace * 3, 0, 0 );
	}
}

//���[���h�s��Z�o.
void CSkinMesh::CalcWorldMatrix()
{
	D3DXMATRIX	mScale, mYaw, mPitch, mRoll, mTran;
	//�g�k.
	D3DXMatrixScaling( &mScale, m_Scale.x, m_Scale.y, m_Scale.z );

	//��].
	D3DXMatrixRotationY( &mYaw, m_Rotation.y );		//Y����].
	D3DXMatrixRotationX( &mPitch, m_Rotation.x );	//X����].
	D3DXMatrixRotationZ( &mRoll, m_Rotation.z );	//Z����].
	m_mRotation = mYaw * mPitch * mRoll;

	//���s�ړ�.
	D3DXMatrixTranslation( &mTran,
		m_Position.x, m_Position.y, m_Position.z );

	//���[���h�s��.
	m_mWorld = mScale * m_mRotation * mTran;
}

//�R���X�^���g�o�b�t�@�i�{�[���j���V�F�[�_�ɓn��.
void CSkinMesh::SendCBufferPerBone(	SKIN_PARTS_MESH* pMesh )
{
	D3D11_MAPPED_SUBRESOURCE pData;

	if( SUCCEEDED(
		m_pContext11->Map(
			m_pCBufferPerBone, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &pData ) ) )
	{
		CBUFFER_PER_BONES cb;

		for( int i = 0; i < pMesh->NumBone; i++ )
		{
			D3DXMATRIX mat = GetCurrentPoseMatrix( pMesh, i );
			D3DXMatrixTranspose( &mat, &mat );
			cb.mBone[i] = mat;
		}
		memcpy_s( pData.pData, pData.RowPitch,
			reinterpret_cast<void*>( &cb ), sizeof( cb ) );

		m_pContext11->Unmap( m_pCBufferPerBone, 0 );
	}
	m_pContext11->VSSetConstantBuffers( enCBSlot::Bones, 1, &m_pCBufferPerBone );
	m_pContext11->PSSetConstantBuffers( enCBSlot::Bones, 1, &m_pCBufferPerBone );
}

//�R���X�^���g�o�b�t�@�i�t���[���j���V�F�[�_�ɓn��.
void CSkinMesh::SendCBufferPerFrame()
{
	D3D11_MAPPED_SUBRESOURCE pData;

	if( SUCCEEDED(
		m_pContext11->Map(
			m_pCBufferPerFrame, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &pData ) ) )
	{
		CBUFFER_PER_FRAME cb;

		//�J�����ʒu.
		cb.CameraPos = D3DXVECTOR4( m_CamPos.x, m_CamPos.y, m_CamPos.z, 0.0f );
		//���C�g����.
		cb.vLightDir = D3DXVECTOR4( m_Light.vDirection.x, m_Light.vDirection.y, m_Light.vDirection.z, 0.0f );
		//���C�g�����̐��K��(�m�[�}���C�Y).
		//	�����f�����烉�C�g�֌���������. �f�B���N�V���i�����C�g�ŏd�v�ȗv�f.
		D3DXVec4Normalize( &cb.vLightDir, &cb.vLightDir );

		memcpy_s( pData.pData, pData.RowPitch,
			reinterpret_cast<void*>( &cb ), sizeof( cb ) );

		m_pContext11->Unmap( m_pCBufferPerFrame, 0 );
	}
	m_pContext11->VSSetConstantBuffers( enCBSlot::Frame, 1, &m_pCBufferPerFrame );
	m_pContext11->PSSetConstantBuffers( enCBSlot::Frame, 1, &m_pCBufferPerFrame );
}

//�R���X�^���g�o�b�t�@�i���b�V���j���V�F�[�_�ɓn��.
void CSkinMesh::SendCBufferPerMesh()
{
	D3D11_MAPPED_SUBRESOURCE pDat;

	if( SUCCEEDED(
		m_pContext11->Map(
			m_pCBufferPerMesh, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &pDat )))
	{
		CBUFFER_PER_MESH cb;
		cb.mW = m_mWorld;
		D3DXMatrixTranspose( &cb.mW, &cb.mW );		//�s���]�u����.
		cb.mWVP = m_mWorld * m_mView * m_mProj;
		D3DXMatrixTranspose( &cb.mWVP, &cb.mWVP );	//�s���]�u����.
		//���s��̌v�Z���@��DirectX��GPU�ňقȂ邽�ߓ]�u���K�v.

		memcpy_s( pDat.pData, pDat.RowPitch,
			reinterpret_cast<void*>( &cb ), sizeof( cb ));
		m_pContext11->Unmap( m_pCBufferPerMesh, 0 );
	}

	m_pContext11->VSSetConstantBuffers( enCBSlot::Mesh, 1, &m_pCBufferPerMesh );
	m_pContext11->PSSetConstantBuffers( enCBSlot::Mesh, 1, &m_pCBufferPerMesh );
}

//�R���X�^���g�o�b�t�@�i�}�e���A���j���V�F�[�_�ɓn��.
void CSkinMesh::SendCBufferPerMaterial( MY_SKINMATERIAL* pMaterial )
{
	D3D11_MAPPED_SUBRESOURCE pDat;

	if( SUCCEEDED(
		m_pContext11->Map(
			m_pCBufferPerMaterial, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &pDat )))
	{
		CBUFFER_PER_MATERIAL cb;
		cb.Ambient	= pMaterial->Ambient;
		cb.Diffuse	= pMaterial->Diffuse;
		cb.Specular	= pMaterial->Specular;

		memcpy_s( pDat.pData, pDat.RowPitch,
			reinterpret_cast<void*>( & cb ), sizeof( cb ));

		m_pContext11->Unmap( m_pCBufferPerMaterial, 0 );
	}

	m_pContext11->VSSetConstantBuffers( enCBSlot::Material, 1, &m_pCBufferPerMaterial );
	m_pContext11->PSSetConstantBuffers( enCBSlot::Material, 1, &m_pCBufferPerMaterial );
}

//�e�N�X�`�����V�F�[�_�ɓn��.
void CSkinMesh::SendTexture( MY_SKINMATERIAL* pMaterial )
{
	if( pMaterial->TextureName != nullptr )
	{
		m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinear );
		m_pContext11->PSSetShaderResources( 0, 1, &pMaterial->pTexture );
	}
	else
	{
		ID3D11ShaderResourceView* Nothing[1] = { 0 };
		m_pContext11->PSSetShaderResources( 0, 1, Nothing );
	}
}

//����֐�.
HRESULT CSkinMesh::Release()
{
	if( m_pD3dxMesh != nullptr ){
		//�S�Ẵ��b�V���폜.
		DestroyAllMesh( m_pD3dxMesh->m_pFrameRoot );

		//�p�[�T�[�N���X�������.
		m_pD3dxMesh->Release();
		SAFE_DELETE( m_pD3dxMesh );
	}

	return S_OK;
}


//�S�Ẵ��b�V�����폜.
void CSkinMesh::DestroyAllMesh( D3DXFRAME* pFrame )
{
	if ((pFrame != nullptr) && (pFrame->pMeshContainer != nullptr))
	{
		DestroyAppMeshFromD3DXMesh( pFrame );
	}

	//�ċA�֐�.
	if( pFrame->pFrameSibling != nullptr )
	{
		DestroyAllMesh( pFrame->pFrameSibling );
	}
	if( pFrame->pFrameFirstChild != nullptr )
	{
		DestroyAllMesh( pFrame->pFrameFirstChild );
	}
}


//���b�V���̍폜.
HRESULT CSkinMesh::DestroyAppMeshFromD3DXMesh( LPD3DXFRAME p )
{
	MYFRAME* pFrame = reinterpret_cast<MYFRAME*>( p );

	MYMESHCONTAINER* pMeshContainerTmp = reinterpret_cast<MYMESHCONTAINER*>( pFrame->pMeshContainer );

	//MYMESHCONTAINER�̒��g�̉��.
	if ( pMeshContainerTmp != nullptr )
	{
		if ( pMeshContainerTmp->pBoneBuffer != nullptr )
		{
			pMeshContainerTmp->pBoneBuffer->Release();
			pMeshContainerTmp->pBoneBuffer = nullptr;
		}

		if ( pMeshContainerTmp->pBoneOffsetMatrices != nullptr )
		{
			delete pMeshContainerTmp->pBoneOffsetMatrices;
			pMeshContainerTmp->pBoneOffsetMatrices = nullptr;
		}
		
		if ( pMeshContainerTmp->ppBoneMatrix != nullptr )
		{
			int iMax = static_cast<int>( pMeshContainerTmp->pSkinInfo->GetNumBones() );

			for ( int i = iMax - 1; i >= 0; i-- )
			{
				if ( pMeshContainerTmp->ppBoneMatrix[i] != nullptr )
				{
					pMeshContainerTmp->ppBoneMatrix[i] = nullptr;
				}
			}

			delete[] pMeshContainerTmp->ppBoneMatrix;
			pMeshContainerTmp->ppBoneMatrix = nullptr;
		}

		if ( pMeshContainerTmp->ppTextures != nullptr )
		{
			int iMax = static_cast<int>( pMeshContainerTmp->NumMaterials );

			for ( int i = iMax - 1; i >= 0; i-- )
			{
				if ( pMeshContainerTmp->ppTextures[i] != nullptr )
				{
					pMeshContainerTmp->ppTextures[i]->Release();
					pMeshContainerTmp->ppTextures[i] = nullptr;
				}
			}

			delete[] pMeshContainerTmp->ppTextures;
			pMeshContainerTmp->ppTextures = nullptr;
		}
	}

	pMeshContainerTmp = nullptr;

	//MYMESHCONTAINER�������.

	//LPD3DXMESHCONTAINER�̉��.
	if ( pFrame->pMeshContainer->pAdjacency != nullptr )
	{
		delete[] pFrame->pMeshContainer->pAdjacency;
		pFrame->pMeshContainer->pAdjacency = nullptr;
	}

	if ( pFrame->pMeshContainer->pEffects != nullptr )
	{
		if ( pFrame->pMeshContainer->pEffects->pDefaults != nullptr )
		{
			if ( pFrame->pMeshContainer->pEffects->pDefaults->pParamName != nullptr )
			{
				delete pFrame->pMeshContainer->pEffects->pDefaults->pParamName;
				pFrame->pMeshContainer->pEffects->pDefaults->pParamName = nullptr;
			}

			if ( pFrame->pMeshContainer->pEffects->pDefaults->pValue != nullptr )
			{
				delete pFrame->pMeshContainer->pEffects->pDefaults->pValue;
				pFrame->pMeshContainer->pEffects->pDefaults->pValue = nullptr;
			}

			delete pFrame->pMeshContainer->pEffects->pDefaults;
			pFrame->pMeshContainer->pEffects->pDefaults = nullptr;
		}

		if ( pFrame->pMeshContainer->pEffects->pEffectFilename != nullptr )
		{
			delete pFrame->pMeshContainer->pEffects->pEffectFilename;
			pFrame->pMeshContainer->pEffects->pEffectFilename = nullptr;
		}

		delete pFrame->pMeshContainer->pEffects;
		pFrame->pMeshContainer->pEffects = nullptr;
	}

	if ( pFrame->pMeshContainer->pMaterials != nullptr )
	{
		int iMax = static_cast<int>( pFrame->pMeshContainer->NumMaterials );

		for ( int i = iMax - 1; i >= 0; i-- )
		{
			delete[] pFrame->pMeshContainer->pMaterials[i].pTextureFilename;
			pFrame->pMeshContainer->pMaterials[i].pTextureFilename = nullptr;
		}

		delete[] pFrame->pMeshContainer->pMaterials;
		pFrame->pMeshContainer->pMaterials = nullptr;
	}

	if ( pFrame->pMeshContainer->pNextMeshContainer != nullptr )
	{
		//���̃��b�V���R���e�i�[�̃|�C���^�[�����̂��Ƃ�����.
		//new�ō���邱�Ƃ͂Ȃ��͂��Ȃ̂ŁA����ōs����Ǝv��.
		pFrame->pMeshContainer->pNextMeshContainer = nullptr;
	}

	if ( pFrame->pMeshContainer->pSkinInfo != nullptr )
	{
		pFrame->pMeshContainer->pSkinInfo->Release();
		pFrame->pMeshContainer->pSkinInfo = nullptr;
	}

	//LPD3DXMESHCONTAINER�̉������.

	//MYFRAME�̉��.

	if ( pFrame->pPartsMesh != nullptr )
	{
		//�{�[�����̉��.
		if ( pFrame->pPartsMesh->pBoneArray != nullptr )
		{
			delete[] pFrame->pPartsMesh->pBoneArray;
			pFrame->pPartsMesh->pBoneArray = nullptr;
		}

		if ( pFrame->pPartsMesh->pMaterial != nullptr )
		{
			int iMax = static_cast<int>( pFrame->pPartsMesh->NumMaterial );

			for ( int i = iMax - 1; i >= 0; i-- )
			{
				if ( pFrame->pPartsMesh->pMaterial[i].pTexture != nullptr )
				{
					pFrame->pPartsMesh->pMaterial[i].pTexture->Release();
					pFrame->pPartsMesh->pMaterial[i].pTexture = nullptr;
				}
			}

			delete[] pFrame->pPartsMesh->pMaterial;
			pFrame->pPartsMesh->pMaterial = nullptr;
		}
		

		if ( pFrame->pPartsMesh->ppIndexBuffer != nullptr )
		{
			//�C���f�b�N�X�o�b�t�@���.
			for ( DWORD i = 0; i < pFrame->pPartsMesh->NumMaterial; i++ ){
				if ( pFrame->pPartsMesh->ppIndexBuffer[i] != nullptr ){
					pFrame->pPartsMesh->ppIndexBuffer[i]->Release();
					pFrame->pPartsMesh->ppIndexBuffer[i] = nullptr;
				}
			}
			delete[] pFrame->pPartsMesh->ppIndexBuffer;
		}

		//���_�o�b�t�@�J��.
		pFrame->pPartsMesh->pVertexBuffer->Release();
		pFrame->pPartsMesh->pVertexBuffer = nullptr;
	}

	//�p�[�c�}�e���A���J��.
	delete[] pFrame->pPartsMesh;
	pFrame->pPartsMesh = nullptr;

	//SKIN_PARTS_MESH�������.

	//MYFRAME��SKIN_PARTS_MESH�ȊO�̃����o�[�|�C���^�[�ϐ��͕ʂ̊֐��ŉ������Ă���.

	return S_OK;
}



//�A�j���[�V�����Z�b�g�̐؂�ւ�.
void CSkinMesh::ChangeAnimSet( int index, LPD3DXANIMATIONCONTROLLER pAC )
{
	if( m_pD3dxMesh == nullptr ){
		return;
	}
	m_pD3dxMesh->ChangeAnimSet( index, pAC );
}


//�A�j���[�V�����Z�b�g�̐؂�ւ�(�J�n�t���[���w��\��).
void CSkinMesh::ChangeAnimSet_StartPos( int index, double dStartFramePos, LPD3DXANIMATIONCONTROLLER pAC )
{
	if( m_pD3dxMesh == nullptr ){
		return;
	}
	m_pD3dxMesh->ChangeAnimSet_StartPos( index, dStartFramePos, pAC );
}


//�A�j���[�V������~���Ԃ��擾.
double CSkinMesh::GetAnimPeriod( int index )
{
	if( m_pD3dxMesh == nullptr ){
		return 0.0f;
	}
	return m_pD3dxMesh->GetAnimPeriod( index );
}


//�A�j���[�V���������擾.
int CSkinMesh::GetAnimMax( LPD3DXANIMATIONCONTROLLER pAC )
{
	if( m_pD3dxMesh != nullptr ){
		return m_pD3dxMesh->GetAnimMax( pAC );
	}
	return -1;
}


//�w�肵���{�[�����(�s��)���擾����֐�.
bool CSkinMesh::GetMatrixFromBone(
	LPCSTR sBoneName, D3DXMATRIX* pOutMat )
{
	if( m_pD3dxMesh != nullptr ){
		if( m_pD3dxMesh->GetMatrixFromBone( sBoneName, pOutMat ) ){
			return true;
		}
	}
	return false;
}
//�w�肵���{�[�����(���W)���擾����֐�.
bool CSkinMesh::GetPosFromBone(
	LPCSTR sBoneName, D3DXVECTOR3* pOutPos )
{
	if( m_pD3dxMesh != nullptr )
	{
		D3DXVECTOR3 tmpPos;
		if( m_pD3dxMesh->GetPosFromBone( sBoneName, &tmpPos ) ){
			D3DXMATRIX mWorld, mScale, mTran;
			D3DXMATRIX mRot, mYaw, mPitch, mRoll;
			D3DXMatrixScaling( &mScale, m_Scale.x, m_Scale.y, m_Scale.z );
			D3DXMatrixRotationY( &mYaw, m_Rotation.y );
			D3DXMatrixRotationX( &mPitch, m_Rotation.x );
			D3DXMatrixRotationZ( &mRoll, m_Rotation.z );
			D3DXMatrixTranslation( &mTran, tmpPos.x, tmpPos.y, tmpPos.z );

			mRot = mYaw * mPitch * mRoll;
			mWorld = mTran * mScale * mRot;

			pOutPos->x = mWorld._41 + m_Position.x;
			pOutPos->y = mWorld._42 + m_Position.y;
			pOutPos->z = mWorld._43 + m_Position.z;

			return true;
		}
	}
	return false;
}

bool CSkinMesh::GetDeviaPosFromBone( 
	LPCSTR sBoneName, D3DXVECTOR3* pOutPos, D3DXVECTOR3 SpecifiedPos )
{
	if ( m_pD3dxMesh != nullptr )
	{
		D3DXMATRIX mtmp;
		if ( m_pD3dxMesh->GetMatrixFromBone( sBoneName, &mtmp )){
			D3DXMATRIX mWorld, mScale, mTran, mDevia;
			D3DXMATRIX mRot, mYaw, mPitch, mRoll;
			//�����炭�{�[���̏����̌��������ʒu�ɂȂ��Ă���͂�.
			//���炵���������̍s����쐬.
			D3DXMatrixTranslation( &mDevia, SpecifiedPos.x, SpecifiedPos.y, SpecifiedPos.z );
			//�{�[���ʒu�s��Ƃ��炵���������s����|�����킹��.
			D3DXMatrixMultiply( &mtmp, &mDevia, &mtmp );
			//�ʒu�̂ݎ擾.
			D3DXVECTOR3 tmpPos = D3DXVECTOR3( mtmp._41, mtmp._42, mtmp._43 );

			D3DXMatrixScaling( &mScale, m_Scale.x, m_Scale.y, m_Scale.z );
			D3DXMatrixRotationY( &mYaw, m_Rotation.y );
			D3DXMatrixRotationX( &mPitch, m_Rotation.x );
			D3DXMatrixRotationZ( &mRoll, m_Rotation.z );
			D3DXMatrixTranslation( &mTran, tmpPos.x, tmpPos.y, tmpPos.z );

			mRot = mYaw * mPitch * mRoll;
			mWorld = mTran * mScale * mRot;

			pOutPos->x = mWorld._41 + m_Position.x;
			pOutPos->y = mWorld._42 + m_Position.y;
			pOutPos->z = mWorld._43 + m_Position.z;

			return true;
		}
	}
	return false;
}


//�R���X�^���g�o�b�t�@�쐬�֐�.
HRESULT CSkinMesh::CreateCBuffer(
	ID3D11Buffer** pConstantBuffer,
	UINT Size)
{
	D3D11_BUFFER_DESC cb;
	cb.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth			= Size;
	cb.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags			= 0;
	cb.StructureByteStride	= 0;
	cb.Usage				= D3D11_USAGE_DYNAMIC;

	if (FAILED(
		m_pDevice11->CreateBuffer( &cb, nullptr, pConstantBuffer )))
	{
		return E_FAIL;
	}
	return S_OK;
}

//�T���v���[�쐬�֐�.
HRESULT CSkinMesh::CreateSampler( ID3D11SamplerState** pSampler )
{
	//�e�N�X�`���[�p�T���v���[�쐬.
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory( &SamDesc, sizeof( D3D11_SAMPLER_DESC ) );

	SamDesc.Filter	= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	if (FAILED(
		m_pDevice11->CreateSamplerState( &SamDesc, &m_pSampleLinear )))
	{
		return E_FAIL;
	}
	return S_OK;
}

//�}���`�o�C�g������Unicode�����ɕϊ�����.
void CSkinMesh::ConvertCharaMultiByteToUnicode(
	WCHAR* Dest,			//(out)�ϊ���̕�����(Unicode������).
	size_t DestArraySize,	//�ϊ���̕�����̔z��̗v�f�ő吔.
	const CHAR* str )		//(in)�ϊ��O�̕�����(�}���`�o�C�g������).
{
	//�e�N�X�`�����̃T�C�Y���擾.
	size_t charSize = strlen( str ) + 1;
	size_t ret;	//�ϊ����ꂽ������.

	//�}���`�o�C�g�����̃V�[�P���X��Ή����郏�C�h�����̃V�[�P���X�ɕϊ����܂�.
	errno_t err = mbstowcs_s(
		&ret,
		Dest,
		charSize,
		str,
		_TRUNCATE );
}