/**************************************************
*	レイ（線分）描画クラス用シェーダファイル
**/
//グローバル変数
//コンスタントバッファ
//アプリ側と同じバッファサイズになっている必要がある
cbuffer per_mesh : register(b0) //レジスタ番号
{
    matrix g_mWVP;  //ワールド,ビュー,プロジェクションの変換合成行列
    float4 g_Color; //色(RGBA:xyzw)
};

//頂点シェーダ
//主にモデルの頂点の描画位置を決定する
float4 VS_Main( float4 Pos : POSITION ) : SV_POSITION
{
	return Pos;
}

//ピクセルシェーダ
//画面上に評されるピクセル（ドット１つ分）の色を決定する
float4 PS_Main(float4 Pos : SV_POSITION) : SV_Target
{
    return g_Color;
}