#include "NumberImage.h"

NumberImage::NumberImage()
	: m_DigitObject			()
	, m_DigitPattern_Map	()
	, m_Digits				()
	, m_DigitWidth			()
	, m_BasePos				(D3DXVECTOR2{WND_W / 2, WND_H / 2 - 16 })
{
	//初期化.
	for (int i = 0; i < 10; i++)
	{
		m_DigitPattern_Map[i] = POINT{ i, 0 };
	}
}

NumberImage::~NumberImage()
{
}

//更新処理.
void NumberImage::Update()
{
	//行数に応じてオブジェクトの数を調整.
	if (m_DigitObject.size() < m_Digits.size())
	{
		for (int i = m_DigitObject.size(); i < m_Digits.size(); i++)
		{
			auto obj = std::make_shared<CUIObject>();
			obj->AttachSprite(m_pSprite);
			obj->SetRotation(0.0f, 0.0f, 0.0f);
			obj->SetScale(1.0f, 1.0f, 1.0f);
			m_DigitObject.push_back(obj);
		}
	}

	//位置とパターン設定.
	for (int i = 0; i < m_Digits.size(); i++)
	{
		int digitValue = m_Digits[i];
		POINT pattern = m_DigitPattern_Map[digitValue];

		//パターンを設定.
		m_DigitObject[i]->SetPatternNo(pattern.x, pattern.y);

		//座標設定.
		m_DigitObject[i]->SetPosition(
			{
				m_BasePos.x + i * m_DigitWidth,
				m_BasePos.y,
				0
			}
		);

	}
}

//描画処理.
void NumberImage::Draw()
{
	for (auto& digit : m_DigitObject)
	{
		digit->Draw();
	}
}

//初期化.
void NumberImage::Init()
{
}

void NumberImage::SetNumber(int num, int digitsNum)
{
	//中身を消しておく.
	m_Digits.clear();

	if (num == 0)
	{
		m_Digits.push_back(0);
		return;
	}
	else
	{
		while (num > 0)
		{
			//例：num = 60s の場合.
			// 1回目：num % 10 = 0 ※1つ目の要素は{0}で、num / 10 = 6 になる.
			// 2回目：num % 10 = 6 ※2つ目の要素は{6}で、num / 10 = 0になる.
			// vectorの中身は、{0, 6}になる。.
			m_Digits.push_back(num % 10);
			num /= 10;
		}
	}
	// std::reverse：vectorの中の並びをひっくり返す※{0, 6}なので、これをひっくり返す.
	std::reverse(m_Digits.begin(), m_Digits.end());

	//桁数が足りない時、0を追加する.
	while (m_Digits.size() < digitsNum)
	{
		//std::vector.insert：任意の位置に新たな要素を挿入する.
		m_Digits.insert(m_Digits.begin(), 0);
	}

}