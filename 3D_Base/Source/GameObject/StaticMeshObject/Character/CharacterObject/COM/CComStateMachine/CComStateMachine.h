#pragma once
#include <functional>

/*
	ó‘ÔŠÇ—ƒNƒ‰ƒX
	‚Ç‚Ìó‘Ô‚©‚Ì•Û‚âó‘Ô‘JˆÚ‚ÌğŒ‚ğƒ`ƒFƒbƒN‚·‚é
	ó‘ÔŒp‘±ŠÔ‚ÌŠÇ—
*/

class CComStateMachine
{
public:

	//COM‚Ìó‘Ô
	enum class State
	{
		Seek,	//’Tõ
		Chase,	//’Ç”ö
		Attack,	//UŒ‚
		Evade,	//‰ñ”ğ
	};

	//ó‘Ô‘JˆÚ‚Ìİ’è\‘¢‘Ì
	struct TransitionConfig
	{
		float keepDistance = 9.0f;				//•Û‚Â‹——£
		float evadeMultiplier = 0.60;			//‰ñ”ğ—p‚Ì”{—¦
		float attackEnterMultiplier = 1.05f;	//UŒ‚ó‘Ô‚É“ü‚é‚Æ‚«‚Ì‹——£”{—¦
		float attackExitMultiplier = 1.25f;		//UŒ‚ó‘Ô‚©‚ç”²‚¯‚é”{—¦
		int loseFrames = 120;					//Œ©¸‚Á‚½‚Æ”»’è‚·‚é‚Ü‚Å‚ÌƒtƒŒ[ƒ€”
	};

	CComStateMachine();

	void Update();

	//ó‹µ‚ğŒ©‚ÄAó‘Ô‚ğØ‘Ö‚·‚×‚«‚©‚ğ•]‰¿‚·‚é
	void EvaluateTransitions(float distToTarget2, bool hasTarget, int lostSightFrames);

	void ChangeState(State newState);
	State GetState() const { return m_State; }
	int GetStateFrames() const { return m_StateFrames; }

	void SetConfig(const TransitionConfig& config) { m_Config = config; }
	TransitionConfig& GetConfig() { return m_Config; }

	const char* GetStateName() const;

private:
	State m_State;
	int m_StateFrames;
	TransitionConfig m_Config;
};