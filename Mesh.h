#pragma once

//
// ���b�V��
//

// �}�`�`��
#include "Shape.h"

class Mesh : public Shape
{
  // �`��Ɏg�����_�o�b�t�@�I�u�W�F�N�g
  GLuint vbo[3];

  // ���b�V���̕�
  const GLsizei slices;
  
  // ���b�V���̍���
  const GLsizei stacks;
  
  // �f�[�^�Ƃ��ĕێ����钸�_��
  const GLsizei vertices;
  
  // ���ۂɕ`�悷�钸�_��
  const GLsizei indexes;
  
  // �J���[�f�[�^�̃e�N�X�`�����W�̓ǂݎ�茳
  const GLfloat (*texcoord)[2];
  
  // �e�N�X�`�����W�𐶐����ăo�C���h����Ă���o�b�t�@�I�u�W�F�N�g�ɓ]������
  void genCoord();

public:

  // �R���X�g���N�^
  Mesh(int stacks, int slices, const GLfloat (*texcoord)[2] = NULL);

  // �f�X�g���N�^
  virtual ~Mesh();

  // �J���[�f�[�^�̃e�N�X�`�����W�̓ǂݎ�茳��ݒ肷��
  void setTexcoord(GLfloat (*coord)[2])
  {
    texcoord = coord;
  }

  // �`��
  virtual void draw() const;
};
