#pragma once

//インターフェースクラス

class INode
{
public:

	enum class NodeResult
	{
		Idle,		//待機中
		Running,	//実行中
		Success,	//成功
		Fail		//失敗
	};



	//仮想デストラクタ
	virtual ~INode() = default;

	//初期化
	virtual void Init() = 0;

	virtual void Tick() = 0;

	// 後処理
	virtual void Finalize() = 0;
	// ノードの状態を取得
//	virtual NodeResult get_node_result() const = 0;
};