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

  // �f�v�X�f�[�^����J�������W�����߂�Ƃ��ɗp����ꎞ������
  GLfloat (*position)[3];

  // �J���[�f�[�^
  IColorFrameSource *colorSource;
  IColorFrameReader *colorReader;
  IFrameDescription *colorDescription;

  // �J���[�f�[�^�̕ϊ��ɗp����ꎞ������
  GLubyte *color;

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
  GLuint getDepth() const;

  // �J�������W���擾����
  GLuint getPoint() const;

  // �J���[�f�[�^���擾����
  GLuint getColor() const;

  // �g�p���Ă���Z���T�[�̐��𒲂ׂ�
  int getActivated() const
  {
    return sensor != NULL ? 1 : 0;
  }
};
