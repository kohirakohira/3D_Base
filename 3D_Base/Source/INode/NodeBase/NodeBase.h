#pragma once
#include "INode/INode.h" 

#if 1
class NodeBase : public INode {
protected:
	explicit NodeBase(BlackBoard* black_board) : mpBlackBoard{ black_board } {}
	virtual ~NodeBase() = default;

	virtual void Init() override { mNodeResult = NodeResult::Running; }

	virtual void Tick() override {}

	virtual void Finalize() override {}

	virtual NodeResult get_node_result() const { return mNodeResult; }

	NodeResult mNodeResult = NodeResult::Idle; // ノードの状態
	BlackBoard* mpBlackBoard = nullptr; // ブラックボード
};
#endif