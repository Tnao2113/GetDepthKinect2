#include "KinectV2.h"

//
// �[�x�Z���T�֘A�̏���
//

// �W�����C�u����
#include <cassert>

// Kinect �֘A
#pragma comment(lib, "Kinect20.lib")

// �R���X�g���N�^
KinectV2::KinectV2()
{
  // �Z���T���擾����
  if (GetDefaultKinectSensor(&sensor) == S_OK)
  {
    // �Z���T�̎g�p���J�n����
    assert(sensor->Open() == S_OK);

    // ���W�̃}�b�s���O
    assert(sensor->get_CoordinateMapper(&coordinateMapper) == S_OK);

    // �f�v�X�f�[�^�̓ǂݍ��ݐݒ�
    assert(sensor->get_DepthFrameSource(&depthSource) == S_OK);
    assert(depthSource->OpenReader(&depthReader) == S_OK);
    assert(depthSource->get_FrameDescription(&depthDescription) == S_OK);

    // �f�v�X�f�[�^�̃T�C�Y�𓾂�
    depthDescription->get_Width(&depthWidth);
    depthDescription->get_Height(&depthHeight);

    // �f�v�X�f�[�^�̉�f�������߂�
    depthCount = depthWidth * depthHeight;

    // �f�v�X�f�[�^���i�[����e�N�X�`������������
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, depthWidth, depthHeight, 0, GL_RED, GL_UNSIGNED_SHORT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // �f�v�X�f�[�^���狁�߂��J�������W���i�[����e�N�X�`������������
    glGenTextures(1, &pointTexture);
    glBindTexture(GL_TEXTURE_2D, pointTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, depthWidth, depthHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // �J���[�f�[�^�̓ǂݍ��ݐݒ�
    assert(sensor->get_ColorFrameSource(&colorSource) == S_OK);
    assert(colorSource->OpenReader(&colorReader) == S_OK);
    assert(colorSource->get_FrameDescription(&colorDescription) == S_OK);

    // �J���[�f�[�^�̃T�C�Y�𓾂�
    colorDescription->get_Width(&colorWidth);
    colorDescription->get_Height(&colorHeight);

    // �J���[�f�[�^�̉�f�������߂�
    colorCount = colorWidth * colorHeight;

    // �J���[�f�[�^���i�[����e�N�X�`������������
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, colorWidth, colorHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // �f�v�X�f�[�^�̉�f�ʒu�̃J���[�̃e�N�X�`�����W���i�[����o�b�t�@�I�u�W�F�N�g����������
    glGenBuffers(1, &coordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
    glBufferData(GL_ARRAY_BUFFER, depthCount * 2 * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

    // �f�v�X�f�[�^����J�������W�����߂�Ƃ��ɗp����ꎞ���������m�ۂ���
    position = new GLfloat[depthCount][3];

    // �J���[�f�[�^��ϊ�����p����ꎞ���������m�ۂ���
    color = new GLubyte[colorCount * 4];
  }
}

// �f�X�g���N�^
KinectV2::~KinectV2()
{
  if (sensor != NULL)
  {
    // �f�[�^�ϊ��p�̃��������폜����
    delete[] position;
    delete[] color;

    // �o�b�t�@�I�u�W�F�N�g���폜����
    glDeleteBuffers(1, &coordBuffer);

    // �e�N�X�`�����폜����
    glDeleteTextures(1, &depthTexture);
    glDeleteTextures(1, &colorTexture);
    glDeleteTextures(1, &pointTexture);

    // �Z���T���J������
    colorDescription->Release();
    colorReader->Release();
    colorSource->Release();
    depthDescription->Release();
    depthReader->Release();
    depthSource->Release();
    coordinateMapper->Release();
    sensor->Close();
    sensor->Release();
  }
}

// �f�v�X�f�[�^���擾����
bool KinectV2::getDepth() const
{
  // �f�v�X�̃e�N�X�`�����w�肷��
  glBindTexture(GL_TEXTURE_2D, depthTexture);

  // ���̃f�v�X�̃t���[���f�[�^���������Ă����
  IDepthFrame *depthFrame;
  if (depthReader->AcquireLatestFrame(&depthFrame) == S_OK)
  {
    // �f�v�X�f�[�^�̃T�C�Y�Ɗi�[�ꏊ�𓾂�
    UINT depthSize;
    UINT16 *depthBuffer;
    depthFrame->AccessUnderlyingBuffer(&depthSize, &depthBuffer);

    // �J���[�̃e�N�X�`�����W�����߂ē]������
    glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
    ColorSpacePoint *const texcoord(static_cast<ColorSpacePoint *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
    coordinateMapper->MapDepthFrameToColorSpace(depthCount, depthBuffer, depthCount, texcoord);
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // �f�v�X�f�[�^���e�N�X�`���ɓ]������
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, depthWidth, depthHeight, GL_RED, GL_UNSIGNED_SHORT, depthBuffer);

    // �f�v�X�t���[�����J������
    depthFrame->Release();

    return true;
  }

  return false;
}

// �J�������W���擾����
bool KinectV2::getPoint() const
{
  // �J�������W�̃e�N�X�`�����w�肷��
  glBindTexture(GL_TEXTURE_2D, pointTexture);

  // ���̃f�v�X�̃t���[���f�[�^���������Ă����
  IDepthFrame *depthFrame;
  if (depthReader->AcquireLatestFrame(&depthFrame) == S_OK)
  {
    // �f�v�X�f�[�^�̃T�C�Y�Ɗi�[�ꏊ�𓾂�
    UINT depthSize;
    UINT16 *depthBuffer;
    depthFrame->AccessUnderlyingBuffer(&depthSize, &depthBuffer);

    UINT32 entry;
    PointF *table;
    coordinateMapper->GetDepthFrameToCameraSpaceTable(&entry, &table);
    for (unsigned int i = 0; i < entry; ++i)
    {
      position[i][2] = depthBuffer[i] == 0.0 ? -10.0f : -0.001f * float(depthBuffer[i]);
      position[i][0] = table[i].X * position[i][2];
      position[i][1] = -table[i].Y * position[i][2];
    }

    // �J���[�̃e�N�X�`�����W�����߂ē]������
    glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
    ColorSpacePoint *const texcoord(static_cast<ColorSpacePoint *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
    coordinateMapper->MapDepthFrameToColorSpace(depthCount, depthBuffer, depthCount, texcoord);
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // �f�v�X�t���[�����J������
    depthFrame->Release();

    // �J�������W���e�N�X�`���ɓ]������
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
      depthWidth, depthHeight, GL_RGB, GL_FLOAT, position);

    return true;
  }

  return false;
}

// �J���[�f�[�^���擾����
bool KinectV2::getColor() const
{
  // �J���[�̃e�N�X�`�����w�肷��
  glBindTexture(GL_TEXTURE_2D, colorTexture);

  // ���̃J���[�̃t���[���f�[�^���������Ă����
  IColorFrame *colorFrame;
  if (colorReader->AcquireLatestFrame(&colorFrame) == S_OK)
  {
    // �J���[�f�[�^���擾���� RGBA �`���ɕϊ�����
    colorFrame->CopyConvertedFrameDataToArray(colorCount * 4,
      static_cast<BYTE *>(color), ColorImageFormat::ColorImageFormat_Rgba);

    // �J���[�t���[�����J������
    colorFrame->Release();

    // �J���[�f�[�^���e�N�X�`���ɓ]������
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
      colorWidth, colorHeight, GL_RGBA, GL_UNSIGNED_BYTE, color);

    return true;
  }

  return false;
}

// �Z���T�̎��ʎq
IKinectSensor *KinectV2::sensor(NULL);
