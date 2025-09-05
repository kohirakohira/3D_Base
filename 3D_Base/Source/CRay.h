#pragma once

//警告についてのコード分析を無効にする.4005:再定義.
#pragma warning(disable:4005)

//レイ構造体
struct RAY
{
	D3DXVECTOR3	Axis;		//軸
	D3DXVECTOR3	Position;	//位置
	float		Length;		//長さ
	float		RotationY;	//Y軸回転

	RAY() : Axis(), Position(), Length(), RotationY() {}
};

/**************************************************
*	レイ（線分）描画クラス.
**/
class CRay
{
public:
	//======================================
	//	構造体.
	//======================================
	//コンスタントバッファのアプリ側の定義.
	//※シェーダ内のコンスタントバッファと一致している必要あり.
	struct SHADER_CONSTANT_BUFFER
	{
		D3DXMATRIX	mWVP;		//ワールド,ビュー,プロジェクションの合成変換行列.	
		D3DXVECTOR4	vColor;		//カラー（RGBAの型に合わせる）.
	};
	//頂点の構造体.
	struct VERTEX
	{
		D3DXVECTOR3 Pos;	//頂点座標.
	};

public:
	CRay();		//コンストラクタ.
	~CRay();	//デストラクタ.

	//初期化.
	HRESULT Init(RAY& pRay);

	//解放.
	void Release();

	//シェーダ作成.
	HRESULT CreateShader();
	//モデル作成.
	HRESULT CreateModel();

	//レンダリング用.
	void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj, RAY Ray);

private:
	ID3D11Device* m_pDevice11;
	ID3D11DeviceContext* m_pContext11;

	ID3D11VertexShader* m_pVertexShader;	//頂点シェーダ.
	ID3D11InputLayout* m_pVertexLayout;	//頂点レイアウト.
	ID3D11PixelShader* m_pPixelShader;		//ピクセルシェーダ.
	ID3D11Buffer* m_pConstantBuffer;	//コンスタントバッファ.

	ID3D11Buffer* m_pVertexBuffer;	//頂点バッファ.

	D3DXVECTOR3		m_vPosition;	//座標.
	D3DXVECTOR3		m_vRotation;	//回転.

	RAY				m_Ray;			//レイ構造体
};