#pragma once

//-----ライブラリ-----
#include <iostream>
#include <DirectXMath.h>		//D3DXを使用しなくてもいいようにするため.

//クォータニオン.
class Quaternions
{
public:
	Quaternions();
	~Quaternions();

	//初期化関数.
	void Init();






public:
	//構造体.
	//角度からラジアンに変換してから渡す.
	struct Quaternion
	{
		float Pitch;	//X軸回転(ピッチ).
		float Yaw;		//Y軸回転(ヨー).
		float Roll;		//Z軸回転(ロール).
	};
	Quaternion	m_Quat;	//クォータニオン.

	//四元数を保存する変数.
	DirectX::XMVECTOR m_QuaternionConversion;
	//任意軸.
	DirectX::XMVECTOR m_ArbitraryAxis;
	//軸角度からの四元数.
	DirectX::XMVECTOR m_QuaternionAxis;
	//合成保管用.
	DirectX::XMVECTOR m_QtoComposition;
	//方向ベクトル.
	DirectX::XMVECTOR m_DirectionVector;
	//回転後のベクトル.
	DirectX::XMVECTOR m_VectorRotated;
	//XMVECTOR->XMFLOAT3変換用.
	DirectX::XMFLOAT3 m_Out;
	//ノーマライズ用：一定の基準に合わせて調整する処理.
	DirectX::XMVECTOR m_VecNormalize;
	//共役用：2つの対象が互いに影響し合って特定の関係性を持つこと.
	DirectX::XMVECTOR m_VecConjugate;
	//マトリックス.
	DirectX::XMMATRIX m_RotMatrix;
	//XMVECTOR->XMFLOAT4変換用.
	DirectX::XMFLOAT4 m_Qtof;

	//変化する角度.
	float m_Angle;




private:

};
