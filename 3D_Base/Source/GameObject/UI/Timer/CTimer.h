#pragma once
#include <iostream>
#include <chrono>		//�^�C�}�[���g�p����.

//-----�f�o�b�N�e�L�X�g-----
#include "Assets//DebugText//CDebugText.h"		// �f�o�b�O�e�L�X�g�N���X

class CTimer
{
public:
	CTimer();
	~CTimer();

	//���ԕ\��.
	void Draw();

	//�f�o�b�O�\��(����)�̃C���X�^���X�ݒ�.
	void SetDebugFont(std::shared_ptr<CDebugText> debug)
	{
		m_pDbgText = debug;
	}

	//�\���ʒu��ݒ肷��悤�̊֐�.
	void SetTimerPosition(float x, float y)
	{
		m_TimerPosition.x = x;
		m_TimerPosition.y = y;
	}


	//�^�C�}�[�̊J�n.
	void StartTimer(int time);

	//�c�莞�Ԃ̎擾.
	int GetRemainingTime() const;

	//�^�C�}�[���I���������ǂ�����Ԃ�.
	bool IsFinished() const;

	//���������Ƃ��ɕK�v�Ȋ֐�.
	void HitTimer();

	//�������Ă���̌o�ߎ���(���G���ԂƂƂ炦�Ă�����).
	int GetElapsed() const;

public:
	//�Q�[�����̎��Ԃ�ێ�����ϐ�.
	int m_Time;

	//�Q�[�����ł̕\���ʒu.
	struct Pos
	{
		float x;
		float y;
	}m_TimerPosition;

	//���������Ƃ��ɕK�v�ȕϐ�.
	std::chrono::steady_clock::time_point	m_HitTime;

	//�^�C�}�[�̊J�n���Ԃ��L�^����.
	std::chrono::steady_clock::time_point	m_StartTimer;

	//�f�o�b�O�e�L�X�g.
	std::shared_ptr<CDebugText>				m_pDbgText;

	//::duration�F���Ԃ̒�����\�����߂̌^.
	std::chrono::duration<int>				m_TotalTime;	//�^�C�}�[�̍ő厞�Ԃ��L�^(�Q�[������)����.
};