#pragma once

//
// �[�x�Z���T�֘A�̏���
//

// Kinect �֘A
#include <Windows.h>
#include <Kinect.h>

// �E�B���h�E�֘A�̏���
#include "Window.h"

// �[�x�Z���T�֘A�̊��N���X
#include "DepthCamera.h"

class KinectV2 : public DepthCamera
{
  // �Z���T�̎��ʎq
  static IKinectSensor *sensor;

  // ���W�̃}�b�s���O
  ICoordinateMapper *coordinateMapper;

  // �f�v�X�f�[�^
  IDepthFrameSource *depthSource;
  IDepthFrameReader *depthReader;
  IFrameDescription *depthDescription;

  // �f�v�X�f�[�^���i�[����e�N�X�`��
  GLuint depthTexture;

  // �f�v�X�f�[�^����ϊ������|�C���g�̃J�������W���i�[����e�N�X�`��
  GLuint pointTexture;

  // �|�C���g�̐��ƃJ�������W�ւ̕ϊ��ɗp����ꎞ�o�b�t�@�̃T�C�Y
  int pointCount, pointSize;

  // �J�������W�ւ̕ϊ��ɗp����ꎞ�o�b�t�@
  GLfloat (*pointBuffer)[3];

  // �f�v�X�f�[�^�̉�f�ɂ�����J���[�f�[�^�̃e�N�X�`�����W
  GLfloat(*texcoord)[2];

  // �J���[�f�[�^
  IColorFrameSource *colorSource;
  IColorFrameReader *colorReader;
  IFrameDescription *colorDescription;

  // �J���[�f�[�^�ϊ��p�̈ꎞ�o�b�t�@
  BYTE *colorBuffer;

  // �J���[�f�[�^���i�[����e�N�X�`��
  GLuint colorTexture;

  // �R�s�[�R���X�g���N�^ (�R�s�[�֎~)
  KinectV2(const KinectV2 &w);

  // ��� (����֎~)
  KinectV2 &operator=(const KinectV2 &w);

public:

  // �R���X�g���N�^
  KinectV2();

  // �f�X�g���N�^
  virtual ~KinectV2();

  // �f�v�X�f�[�^���擾����
  bool getDepth() const;

  // �J�������W���擾����
  bool getPoint() const;

  // �J���[�f�[�^���擾����
  bool getColor() const;

  // �f�v�X�f�[�^�̉𑜓x���擾����
  void getResolution(int *width, int *height) const
  {
    *width = depthWidth;
    *height = depthHeight;
  }

  // �J���[�f�[�^�̃e�N�X�`�����W���擾����
  const GLfloat(*getTexcoord() const)[2]
  {
    return texcoord;
  }

  // �g�p���Ă���Z���T�[�̐��𒲂ׂ�
  int getActivated() const
  {
    return sensor != NULL ? 1 : 0;
  }
};
