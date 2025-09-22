#pragma once
#include <d3dx9.h>

//-----���b�V��-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // �X�^�e�B�b�N���b�V���N���X

/**************************************************
*	�o�E���f�B���O�{�b�N�X�N���X (OBB)
**************************************************/
class CBoundingBox
{
public:
    struct OBB
    {
        D3DXVECTOR3 m_Pos;     // ���S���W
        D3DXVECTOR3 m_Axis[3]; // ���[�J��X, Y, Z���i�P�ʃx�N�g���j
        float       m_Length[3]; // �e�������̔������ix, y, z�j
    };

public:
    CBoundingBox();
    ~CBoundingBox();

    // �o�E���f�B���O�{�b�N�X���쐬
    void CreateBox(const D3DXVECTOR3& center, float width, float height, float depth);

    // ���C���[���b�V���`��
    void Draw(IDirect3DDevice9* device, D3DCOLOR color = D3DCOLOR_XRGB(255, 0, 0));

    // ����OBB�Ƃ̓����蔻��
    bool IsHitOBB(OBB& obb1, OBB& obb2);

    // OBB�̉�]��ݒ�
    void SetRotation(const D3DXMATRIX& matRot);

    // ���݂�OBB���擾
    const OBB& GetOBB() const { return m_OBB; }

    //���S���W���擾����
    const D3DXVECTOR3& GetPosition() const { return m_Position; }
    //���S���W��ݒ肷��
    void SetPosition(const D3DXVECTOR3& Pos) { m_Position = Pos; }

private:
    D3DXVECTOR3		m_Position;	//���S���W
    OBB m_OBB; // ���̃I�u�W�F�N�g��OBB
};
