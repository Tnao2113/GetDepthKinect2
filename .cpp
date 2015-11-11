#include "KinectV1.h"

//
// �[�x�Z���T�֘A�̏���
//

// �W�����C�u����
#include <cassert>

// Kinect �֘A
#pragma comment(lib, "Kinect10.lib")

// �R���X�g���N�^
Sensor::Sensor()
  : texcoord(new GLfloat [DEPTH_W * DEPTH_H][2])
  , nextColorFrameEvent(CreateEvent(NULL, TRUE, FALSE, NULL))
  , nextDepthFrameEvent(CreateEvent(NULL, TRUE, FALSE, NULL))
{
  // �ŏ��̃C���X�^���X�𐶐�����Ƃ�����
  if (activated == 0)
  {
    // �ڑ�����Ă���Z���T�̐��𒲂ׂ�
    NuiGetSensorCount(&connected);
  }

  // �Z���T���ڑ�����Ă���g�p�䐔���ڑ��䐔�ɒB���Ă��Ȃ����
  if (activated < connected)
  {
    // �Z���T�̎g�p���J�n����
    NuiCreateSensorByIndex(activated, &sensor);

    // �Z���T���g�p�\�ł����
    if (sensor->NuiStatus() == S_OK)
    {
      // �Z���T������������ (�J���[�ƃf�v�X���擾����)
      assert(sensor->NuiInitialize(
        NUI_INITIALIZE_FLAG_USES_COLOR |
        NUI_INITIALIZE_FLAG_USES_DEPTH |
        0) == S_OK);

      // �Z���T�̋p������������
      assert(sensor->NuiCameraElevationSetAngle(0L) == S_OK);

      // �f�v�X�X�g���[���̎擾�ݒ�
      assert(sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, RESOLUTION(DEPTH_W, DEPTH_H),
        0, 2, nextDepthFrameEvent, &depthStream) == S_OK);

      // �f�v�X���i�[����e�N�X�`������������
      glGenTextures(1, &depthTexture);
      glBindTexture(GL_TEXTURE_2D, depthTexture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, DEPTH_W, DEPTH_H, 0, GL_RED, GL_UNSIGNED_SHORT, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

      // �J���[�X�g���[���̎擾�ݒ�
      assert(sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, RESOLUTION(COLOR_W, COLOR_H),
        0, 2, nextColorFrameEvent, &colorStream) == S_OK);

      // �J���[���i�[����e�N�X�`������������
      glGenTextures(1, &colorTexture);
      glBindTexture(GL_TEXTURE_2D, colorTexture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, COLOR_W, COLOR_H, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

      // �g�p���Ă���Z���T�̐��𐔂���
      ++activated;
    }
  }
}

// �f�X�g���N�^
Sensor::~Sensor()
{
  // �J���[�̃e�N�X�`�����W�p�̃��������J������
  delete[] texcoord;

  // �Z���T���L���ɂȂ��Ă�����
  if (activated > 0)
  {
    // �Z���T���V���b�g�_�E������
    sensor->NuiShutdown();

    // �e�N�X�`�����폜����
    glDeleteTextures(1, &colorTexture);
    glDeleteTextures(1, &depthTexture);

    // �g�p���Ă���Z���T�̐������炷
    --activated;
  }
}

// �f�[�^���擾����
bool Sensor::getImage(HANDLE event, HANDLE stream,
  GLuint texture, GLsizei width, GLsizei height, GLenum format, GLenum type) const
{
  // �J���[�̃e�N�X�`�����w�肷��
  glBindTexture(GL_TEXTURE_2D, texture);

  // ���̃t���[���f�[�^���������Ă����
  if (WaitForSingleObject(event, 0) != WAIT_TIMEOUT)
  {
    // �擾�����t���[���f�[�^�̊i�[�ꏊ
    NUI_IMAGE_FRAME frame;

    // �t���[���f�[�^�̊i�[�ꏊ�� frame �Ɏ擾����
    if (sensor->NuiImageStreamGetNextFrame(stream, 0, &frame) == S_OK)
    {
      // ���ꂩ�珈�������܂Ńf�[�^���ύX����Ȃ��悤�Ƀ��b�N����
      NUI_LOCKED_RECT rect;
      frame.pFrameTexture->LockRect(0, &rect, NULL, 0);

      // ���b�N�ɐ���������
      if (rect.Pitch)
      {
        // pBits �ɓ����Ă���f�[�^���e�N�X�`���ɓ]������
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, rect.pBits);

        // �擾�����̂��f�v�X�f�[�^�ł����
        if (texture == depthTexture)
        {
          // �J���[�f�[�^�̃e�N�X�`�����W�����߂� texcoord �ɕۑ�����
          for (int i = 0; i < DEPTH_W * DEPTH_H; ++i)
          {
            // �f�v�X�̉�f�ʒu����J���[�̉�f�ʒu�����߂�
            LONG x, y;
            sensor->NuiImageGetColorPixelCoordinatesFromDepthPixel(RESOLUTION(COLOR_W, COLOR_H),
              NULL, i % DEPTH_W, i / DEPTH_W, reinterpret_cast<USHORT *>(rect.pBits)[i], &x, &y);

            // �J���[�f�[�^�̃e�N�X�`�����W�ɕϊ�����
            texcoord[i][0] = (GLfloat(x) + 0.5f) / GLfloat(COLOR_W);
            texcoord[i][1] = (GLfloat(y) + 0.5f) / GLfloat(COLOR_H);
          }
        }

        // ���b�N�����f�[�^���J������
        assert(sensor->NuiImageStreamReleaseFrame(stream, &frame) == S_OK);

        return true;
      }
    }
  }

  return false;
}

// �ڑ����Ă���Z���T�̐�
int Sensor::connected(0);

// �g�p���Ă���Z���T�̐�
int Sensor::activated(0);
