#pragma once
#include <iostream>

//===================================================================
//		シングルトン基底クラス.
//===================================================================
//テンプレート(型パラメータ)：「このクラスはTという"型"を後で注入して使う」という"型の入れ物"みたいなもの.
template <typename T>
class CSingleton
{
public:
	//シングルトン化※インスタンスを一つだけにする部分.
	//一度だけ生成する.
	static T& GetInstance()
	{
		static T instance;
		return instance;
	}

protected:
	//外部からnew(std::make_○○)できないようにする.
	// = default：派生クラスから呼べるようにしたい※自動生成の挙動を明示しておく.
	CSingleton() = default;
	virtual ~CSingleton() = default;
	//コピーコンストラクタ・コピー代入演算子の禁止.
	CSingleton(const CSingleton&) = delete;
	CSingleton& operator = (const CSingleton&) = delete;
};