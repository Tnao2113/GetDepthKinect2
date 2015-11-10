#pragma once

//
// �[�x�Z���T�֘A�̏���
//

// Kinect �֘A
#include <Windows.h>
#include <Kinect.h>

// �E�B���h�E�֘A�̏���
#include "Window.h"

// NUI_IMAGE_RESOLUTION �̐ݒ�
#define EXPAND_RESOLUTION(width, height) NUI_IMAGE_RESOLUTION_##width##x##height
#define RESOLUTION(width, height) EXPAND_RESOLUTION(width, height)

class Sensor
{
  // �Z���T�̎��ʎq
  static IKinectSensor *sensor;

  // ���W�̃}�b�s���O
  ICoordinateMapper *coordinateMapper;

  // �J���[�f�[�^
  IColorFrameSource *colorSource;
  IColorFrameReader *colorReader;
  IFrameDescription *colorDescription;

  // �J���[�f�[�^�̃T�C�Y
  int color_w, color_h, color_size;

  // �J���[�f�[�^�ϊ��p�̈ꎞ�o�b�t�@
  BYTE *color_buffer;

  // �J���[�f�[�^���i�[����e�N�X�`��
  GLuint colorTexture;

  // �f�v�X�f�[�^
  IDepthFrameSource *depthSource;
  IDepthFrameReader *depthReader;
  IFrameDescription *depthDescription;

  // �f�v�X�f�[�^�̃T�C�Y
  int depth_w, depth_h, depth_count;

  // �f�v�X�f�[�^���i�[����e�N�X�`��
  GLuint depthTexture;

  // �`��Ɏg�����_�z��I�u�W�F�N�g
  GLuint vao;

  // �`��Ɏg�����_�o�b�t�@�I�u�W�F�N�g
  GLuint vbo[3];

  // �`�悷��O�p�`�̒��_�̑���
  GLsizei vertexCount;

  // �R�s�[�R���X�g���N�^ (�R�s�[�֎~)
  Sensor(const Sensor &w);

  // ��� (����֎~)
  Sensor &operator=(const Sensor &w);

public:

  // �R���X�g���N�^
  Sensor();

  // �f�X�g���N�^
  virtual ~Sensor();

  // �J���[�f�[�^���擾����
  bool getColor() const;

  // �f�v�X�f�[�^���擾����
  bool getDepth() const;

  // �g�p���Ă���Z���T�[�̐��𒲂ׂ�
  int getActivated() const
  {
    return sensor != NULL ? 1 : 0;
  }

  // �`��
  void draw() const
  {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, NULL);
  }
};
