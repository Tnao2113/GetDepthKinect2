#include "Mesh.h"

//
// ���b�V��
//

// �e�N�X�`�����W�̐������ăo�b�t�@�I�u�W�F�N�g�ɓ]������
void Mesh::genTexcoord()
{
  GLfloat (*const t)[2](static_cast<GLfloat(*)[2]>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
  for (int i = 0; i < vertices; ++i)
  {
    t[i][0] = (GLfloat(i % slices) + 0.5f) / GLfloat(slices);
    t[i][1] = (GLfloat(i / slices) + 0.5f) / GLfloat(stacks);
  }
  glUnmapBuffer(GL_ARRAY_BUFFER);
}

// �R���X�g���N�^
Mesh::Mesh(int slices, int stacks, const GLfloat (*texcoord)[2])
  : slices(slices)
  , stacks(stacks)
  , vertices(slices * stacks)
  , indexes((slices - 1) * (stacks - 1) * 3 * 2)
  , texcoord(texcoord)
{
  // �`��Ɏg�����b�V���f�[�^�̒��_�o�b�t�@�I�u�W�F�N�g����������
  glGenBuffers(sizeof vbo / sizeof vbo[0], vbo);

  // �f�v�X�f�[�^�̃T���v�����O�p�̃o�b�t�@�I�u�W�F�N�g
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, vertices * 2 * sizeof (GLfloat), NULL, GL_STATIC_DRAW);

  // �f�v�X�f�[�^�̃e�N�X�`�����W�����߂ăo�b�t�@�I�u�W�F�N�g�ɓ]������
  genTexcoord();

  // �C���f�b�N�X�� 0 �� varying �ϐ��Ɋ��蓖�Ă�
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  // �J���[�f�[�^���f�v�X�f�[�^�̉�f�P�ʂɃT���v�����O���邽�߂̃o�b�t�@�I�u�W�F�N�g
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, vertices * 2 * sizeof (GLfloat), NULL, GL_DYNAMIC_DRAW);

  // �J���[�f�[�^�̃e�N�X�`�����W�����߂ăo�b�t�@�I�u�W�F�N�g�ɓ]������
  genTexcoord();

  // �C���f�b�N�X�� 1 �� varying �ϐ��Ɋ��蓖�Ă�
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  // �C���f�b�N�X�p�̃o�b�t�@�I�u�W�F�N�g
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes * sizeof (GLuint), NULL, GL_STATIC_DRAW);

  // �C���f�b�N�X�����߂ăo�b�t�@�I�u�W�F�N�g�ɓ]������
  GLuint *index(static_cast<GLuint *>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY)));
  for (int j = 0; j < stacks - 1; ++j)
  {
    for (int i = 0; i < slices - 1; ++i)
    {
      index[0] = slices * j + i;
      index[1] = index[5] = index[0] + 1;
      index[2] = index[4] = index[0] + slices;
      index[3] = index[2] + 1;
      index += 6;
    }
  }
  glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

// �f�X�g���N�^
Mesh::~Mesh()
{
  // ���_�o�b�t�@�I�u�W�F�N�g���폜����
  glDeleteBuffers(sizeof vbo / sizeof vbo[0], vbo);
}

// �`��
void Mesh::draw() const
{
  // �e�N�X�`�����W�̓ǂݎ��悪�ݒ肳��Ă����
  if (texcoord != NULL)
  {
    // �J���[�̃e�N�X�`�����W���o�b�t�@�I�u�W�F�N�g�ɓ]������
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices * 2 * sizeof(GLfloat), texcoord);
  }

  // ���_�z��I�u�W�F�N�g���w�肵�ĕ`�悷��
  Shape::draw();
  glDrawElements(GL_TRIANGLES, indexes, GL_UNSIGNED_INT, NULL);
}
