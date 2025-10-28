/*********************************************************************
*	スタティックメッシュクラス用シェーダファイル.
**/
//グローバル変数.
//テクスチャは レジスタ t(n).
Texture2D		g_Texture	: register( t0 );
//サンプラは レジスタ s(n).
SamplerState	g_SamLinear	: register( s0 );

//コンスタントバッファ.
//メッシュ単位.
cbuffer per_mesh	: register( b0 )
{
	matrix	g_mW;			//ワールド行列.
	matrix	g_mWVP;			//ワールド,ビュー,プロジェクションの合成行列.
};
//マテリアル単位.
cbuffer per_material: register( b1 )
{
	float4	g_Diffuse;		//ディフューズ色(拡散反射色).
    float4	g_Ambient = float4(0.2,0.2,0.2,0.2); //アンビエント色(環境色).
	float4	g_Specular;		//スペキュラ色(鏡面反射色).
};
//フレーム単位.
cbuffer per_frame	: register( b2 )
{
	float4	g_CameraPos;	//カメラ位置(視点位置).光の位置
    float4  g_LightPos;		//光源の位置.wは未使用
    float4	g_Attenuation;	//X:定数項,Y:線形,Z:2乗,w有効距離
    float4 g_LightColor;    //光の色
};


//頂点シェーダの出力パラメータ.
struct VS_OUTPUT
{
	float4	Pos			: SV_Position;
	float3	Normal		: TEXCOORD0;    //ワールド空間の法線
	float2	UV			: TEXCOORD1;    //テクスチャ画像
	float3	LightVec	: TEXCOORD2;	//光源から点へのベクトル
	float3	EyeVector	: TEXCOORD3;	//視線ベクトル
	float4	PosWorld	: TEXCOORD4;    //頂点のワールド座標
	float4	Color		: COLOR;        
};

//-------------------------------------------------
//	頂点(バーテックス)シェーダ.
//-------------------------------------------------

//VS_OUTPUT VS_Main(
//	float4 Pos	: POSITION,
//	float4 Norm	: NORMAL,
//	float2 UV	: TEXCOORD)
//{
//	VS_OUTPUT output = (VS_OUTPUT)0;

//	//プロジェクション変換(ワールド,ビュー,プロジェクション).
//	output.Pos = mul( Pos, g_mWVP );

//	//法線をモデルの姿勢に合わせる.
//	// (モデルが回転すれば法線も回転させる必要があるため).
//	output.Normal = mul( Norm, (float3x3)g_mW );
//	output.Normal = normalize( output.Normal );

//	//ライト方向:
//	// ディレクショナルライトは、どこでも同じ方向.位置は無関係.
//	output.Light = normalize( g_vLightDir );
	
//	output.PosWorld = mul( Pos, g_mW );

//	//視線ベクトル:
//	// ワールド空間上での頂点から頂点へ向かうベクトル.
//	output.EyeVector = normalize( g_CameraPos - output.PosWorld );

//	//テクスチャ座標.
//	output.UV = UV;

//	return output;
//}
VS_OUTPUT VS_Main(float4 Pos : POSITION, float4 Norm : NORMAL, float2 UV : TEXCOORD)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
	
	//クリップ空間変換
    output.Pos = mul(Pos, g_mWVP);
	
	//ワールド座標
    output.PosWorld = mul(Pos, g_mW);
	
	//法線をワールド座標に変換して正規化
    float3x3 nmat = (float3x3) g_mW;
    output.Normal = normalize(mul(Norm.xyz, nmat));
	
	//光源から点へのベクトル
    output.LightVec = (g_LightPos.xyz - output.PosWorld.xyz);
	
	//視線ベクトル
    output.EyeVector = normalize(g_CameraPos.xyz - output.PosWorld.xyz);
	
    output.UV = UV;
    return output;
}


//-------------------------------------------------
//	ピクセルシェーダ.
//-------------------------------------------------
//float4 PS_Main( VS_OUTPUT input ) : SV_Target
//{
//	//テクスチャカラー.
//	float4 texColor = g_Texture.Sample( g_SamLinear, input.UV );
	
//	//環境光　※１.
//	float4 ambient = texColor * g_Ambient;

//	//拡散反射光 ※２.
//	float NL = saturate( dot( input.Normal, input.Light )  * 0.8f + 0.8f);	
//	float4 diffuse = ( g_Diffuse / 3 + texColor / 3 )*NL;

//	//鏡面反射光 ※３.
//	float3 reflect = normalize( 2 * NL * input.Normal - input.Light );
//	float4 specular =
//		pow( saturate( dot( reflect, input.EyeVector ) ), 4 )*g_Specular;

//	//最終色　※１，２，３の合計.
//	float4 Color = ambient + diffuse + specular;
//	return Color;
//}

float4 PS_Main(VS_OUTPUT input): SV_Target
{
    float4 texColor = g_Texture.Sample(g_SamLinear, input.UV);  //テクスチャの色を取得
	
	//距離と方向
    float d = length(input.LightVec);
    float3 L = input.LightVec / max(d, 1e-4);	//正規化.1e-4はゼロ除算
	
	//減衰
    float kc = g_Attenuation.x;
    float kl = g_Attenuation.y;
    float kq = g_Attenuation.z;
    float range = g_Attenuation.w;
	
    float denom = kc + kl * d + kq * d * d;
    denom = max(denom, 1e-4);
    float att = 1.0 / denom;
	
	//範囲減衰
    float rangeAtt = (range > 0.0) ? saturate(1.0 - (d / range)) : 1.0;
    att *= rangeAtt;

    //拡散
    float3 N = normalize(input.Normal);
    float w = 0.6; //0.20.8で調整。大きいほどムラ減
    float NdotL = saturate((dot(N, L) + w) / (1.0 + w));

    float3 lightRGB = g_LightColor.rgb * g_LightColor.a;
    float4 diffuse = (g_Diffuse * 0.5 + texColor * 0.5) * (NdotL * att);
    diffuse.rgb *= lightRGB;

    //鏡面
    float specScale = 0.0; 
    float shininess = max(g_Specular.a, 1.0);
    float3 V = normalize(input.EyeVector);
    float3 H = normalize(L + V);
    float specPow = pow(saturate(dot(N, H)), shininess);
    float3 specRGB = g_Specular.rgb * lightRGB * (specPow * att) * specScale;
    float4 specular = float4(specRGB, 1.0);

    //環境
    float4 ambient = g_Ambient * texColor;

    //合成
    float4 color = ambient + diffuse + specular;
    color.a = texColor.a;
    return color;

}

//========= テクスチャ無し用 ========================================
//-------------------------------------------------
//	頂点(バーテックス)シェーダ.
//-------------------------------------------------
//VS_OUTPUT VS_NoTex(
//	float4 Pos : POSITION,
//	float4 Norm: NORMAL)
//{
//	VS_OUTPUT output = (VS_OUTPUT)0;

//	//プロジェクション変換(ワールド,ビュー,プロジェクション).
//	output.Pos = mul( Pos, g_mWVP );
	
//	//法線をモデルの姿勢に合わせる.
//	// (モデルが回転すれば法線も回転させる必要があるため).
//	output.Normal = mul( Norm, ( float3x3 )g_mW );
//	output.Normal = normalize( output.Normal );
	
//	//ライト方向:
//	// ディレクショナルライトは、どこでも同じ方向.位置は無関係.
//	output.Light = normalize( g_vLightDir );

//	output.PosWorld = mul( Pos, g_mW );

//	//視線ベクトル:
//	// ワールド空間上での頂点から頂点へ向かうベクトル.
//	output.EyeVector = normalize( g_CameraPos - output.PosWorld );


//	return output;
//}

VS_OUTPUT VS_NoTex(float4 Pos : POSITION, float4 Norm: NORMAL)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
	
    output.Pos = mul(Pos, g_mWVP);
    output.PosWorld = mul(Pos, g_mW);
	
    float3x3 nmat = (float3x3) g_mW;
    output.Normal = normalize(mul(Norm.xyz, nmat));
	
    output.LightVec = (g_LightPos.xyz - output.PosWorld.xyz);
    output.EyeVector = normalize(g_CameraPos.xyz - output.PosWorld.xyz);
    return output;

}


//-------------------------------------------------
//	ピクセルシェーダ.
//-------------------------------------------------
//float4 PS_NoTex( VS_OUTPUT input ) : SV_Target
//{
//	//テクスチャカラー.
//	//環境光　※１.
//	float4 ambient = g_Ambient;

//	//拡散反射光 ※２.
//	float NL = saturate( dot( input.Normal, input.Light ) );
//	float4 diffuse = g_Diffuse * NL;

//	//鏡面反射光 ※３.
//	float3 reflect = normalize( 2 * NL * input.Normal - input.Light );
//	float4 specular =
//		pow( saturate( dot( reflect, input.EyeVector ) ), 4 )*g_Specular;

//	//最終色　※１，２，３の合計.
//	float4 Color = ambient + diffuse + specular;
//	return Color;
//}


float4 PS_NoTex(VS_OUTPUT i) : SV_Target
{
    float d = length(i.LightVec);
    float3 L = i.LightVec / max(d, 1e-4);

    float kc = g_Attenuation.x;
    float kl = g_Attenuation.y;
    float kq = g_Attenuation.z;
    float range = g_Attenuation.w;

    float denom = kc + kl * d + kq * d * d;
    denom = max(denom, 1e-4); 
    float att = 1.0 / denom;

    float rangeAtt = (range > 0.0) ? saturate(1.0 - (d / range)) : 1.0;
    att *= rangeAtt;

    float3 N = normalize(i.Normal);
    float w = 0.6;
    float NdotL = saturate((dot(N, L) + w) / (1.0 + w));
    //完全フラットにしたいならfloat NdotL = 1.0;

    float3 lightRGB = g_LightColor.rgb * g_LightColor.a;

    float4 ambient = g_Ambient;
    float4 diffuse = g_Diffuse * (NdotL * att);
    diffuse.rgb *= lightRGB;

    float specScale = 0.0; 
    float shininess = max(g_Specular.a, 1.0);
    float3 V = normalize(i.EyeVector);
    float3 H = normalize(L + V);
    float specPow = pow(saturate(dot(N, H)), shininess);
    float3 specRGB = g_Specular.rgb * lightRGB * (specPow * att) * specScale;
    float4 specular = float4(specRGB, 1.0);

    float4 color = ambient + diffuse + specular;
    color.a = 1.0;
    return color;
}