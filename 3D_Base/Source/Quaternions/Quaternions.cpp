#include "Quaternions.h"

Quaternions::Quaternions()
	: m_QuaternionConversion			()
	, m_ArbitraryAxis					()
	, m_QuaternionAxis					()
	, m_QtoComposition					()
	, m_DirectionVector					()
	, m_VectorRotated					()
	, m_Out								()
	, m_VecNormalize					()
	, m_VecConjugate					()
	, m_RotMatrix						()
	, m_Qtof							()
	, m_Angle							( 0.f )
{
	m_Quat = { 0.f, 0.f, 0.f};
}

Quaternions::~Quaternions()
{
}

//初期化関数.
void Quaternions::Init()
{
	//各角度を設定(x,y,z).
	m_Quat = { 0.f, 0.f, 0.f };

	//三つの回転を一つの四元数(クォータニオン)に変換.
	//角度の「適用順序」は重要!!.
	//Q1.
	m_QuaternionConversion = DirectX::XMQuaternionRotationRollPitchYaw(
		DirectX::XMConvertToRadians(m_Quat.Pitch),
		DirectX::XMConvertToRadians(m_Quat.Yaw),
		DirectX::XMConvertToRadians(m_Quat.Roll));

	//Y軸のベクトル指定.
	m_ArbitraryAxis = DirectX::XMVectorSet( 0.f, 1.f, 0.f, 0.f );

	//角度設定(Y軸を90°回すクォータニオンの作成).
	m_Angle = 90.0f;

	//クォータニオンの作成(軸(ベクトル)と角度(ラジアン値)を使用).
	//Q2.
	m_QuaternionAxis = DirectX::XMQuaternionRotationAxis( m_ArbitraryAxis, DirectX::XMConvertToRadians(m_Angle));

	//二つのクォータニオンを合成(掛け算).
	//掛け算の順番が回転の順番を決める.
	//m_QtoComposition = Q2 * Q1.
	m_QtoComposition = DirectX::XMQuaternionMultiply(m_QuaternionAxis, m_QuaternionConversion);

	//X軸のベクトル指定.
	m_DirectionVector = DirectX::XMVectorSet(1.f, 0.f, 0.f, 0.f);

	//ベクトルを回転させる.
	//四元数回転を効率的に行う.
	m_VectorRotated = DirectX::XMVector3Rotate(m_DirectionVector, m_QtoComposition);

	//XMVECTOR->XMFLOAT3変換.
	DirectX::XMStoreFloat3(&m_Out, m_VectorRotated);

	//正規化.
	m_VecNormalize	= DirectX::XMQuaternionNormalize(m_QtoComposition);
	//共役.
	m_VecConjugate	= DirectX::XMQuaternionConjugate(m_VecNormalize);
	//四元数を回転行列に変換.
	m_RotMatrix		= DirectX::XMMatrixRotationQuaternion(m_VecNormalize);
	//内部式.
	//軸角度→四元数.
	//ベクトルを回す：ベクトルを四元数に拡張.
	//四元数同士の積(Hamilton積).
	//(x1,y1,z1,w1) * (x2,y2,z2,w2) =
	//( w1*x2 + x1*w2 + y1*z2 - z1*y2,
	//  w1*y2 - x1*z2 + y1*w2 + z1*x2,
	//  w1*z2 + x1*y2 - y1*x2 + z1*w2,
	//  w1*w2 - x1*x2 - y1*y2 - z1*z2 )
	//これを参考->https://chatgpt.com/share/68c23e80-28c4-8001-9fb0-b597b8909197#:~:text=%E5%86%85%E9%83%A8%E7%9A%84%E3%81%AA,y2%20%2D%20z1*z2%20

	//XMVECTOR->XMFLOAT3変換.
	//(x,y,z,w) の順で取り出す点に注意.
	DirectX::XMStoreFloat4(&m_Qtof, m_VecNormalize);

}
