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

    // �f�v�X�f�[�^���i�[����e�N�X�`������������
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, depthWidth, depthHeight, 0, GL_RED, GL_UNSIGNED_SHORT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // �f�v�X�f�[�^�ϊ��p�̈ꎞ�o�b�t�@���m�ۂ���
    pointCount = depthWidth * depthHeight;
    pointSize = pointCount * 3 * sizeof (GLfloat);
    pointBuffer = new GLfloat[pointCount][3];

    // �f�v�X�f�[�^���狁�߂��J�������W���i�[����e�N�X�`������������
    glGenTextures(1, &pointTexture);
    glBindTexture(GL_TEXTURE_2D, pointTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, depthWidth, depthHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // �f�v�X�f�[�^�̉�f�ʒu�̃J���[�̃e�N�X�`�����W�ٕۑ����郁�������m�ۂ���
    texcoord = new GLfloat[pointCount][2];

    // �J���[�f�[�^�̓ǂݍ��ݐݒ�
    assert(sensor->get_ColorFrameSource(&colorSource) == S_OK);
    assert(colorSource->OpenReader(&colorReader) == S_OK);
    assert(colorSource->get_FrameDescription(&colorDescription) == S_OK);

    // �J���[�f�[�^�̃T�C�Y�𓾂�
    colorDescription->get_Width(&colorWidth);
    colorDescription->get_Height(&colorHeight);

    // �J���[�f�[�^�ϊ��p�̈ꎞ�o�b�t�@���m�ۂ���
    colorSize = colorWidth * colorHeight * 4;
    colorBuffer = new BYTE[colorSize];

    // �J���[���i�[����e�N�X�`������������
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, colorWidth, colorHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }
}

// �f�X�g���N�^
KinectV2::~KinectV2()
{
  if (sensor != NULL)
  {
    // �f�[�^�ϊ��p�̈ꎞ�o�b�t�@���J������
    delete[] pointBuffer;
    delete[] colorBuffer;

    // �f�v�X�f�[�^�̉�f�ʒu�̃J���[�̃e�N�X�`�����W�ٕۑ����郁�������m�ۂ���
    delete[] texcoord;

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

    // �J���[�̃e�N�X�`�����W�����߂ĕۑ�����
    coordinateMapper->MapDepthFrameToColorSpace(pointCount, depthBuffer, pointCount,
      reinterpret_cast<ColorSpacePoint *>(texcoord));

    // �J�������W���e�N�X�`���ɓ]������
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, depthWidth, depthHeight,
      GL_RED, GL_UNSIGNED_SHORT, depthBuffer);

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
      pointBuffer[i][2] = depthBuffer[i] == 0.0 ? -10.0f : -0.001f * float(depthBuffer[i]);
      pointBuffer[i][0] = table[i].X * pointBuffer[i][2];
      pointBuffer[i][1] = -table[i].Y * pointBuffer[i][2];
    }

    // �J���[�̃e�N�X�`�����W�����߂ĕۑ�����
    coordinateMapper->MapDepthFrameToColorSpace(pointCount, depthBuffer, pointCount,
      reinterpret_cast<ColorSpacePoint *>(texcoord));

    // �f�v�X�t���[�����J������
    depthFrame->Release();

    // �J�������W���e�N�X�`���ɓ]������
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, depthWidth, depthHeight,
      GL_RGB, GL_FLOAT, pointBuffer);

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
    colorFrame->CopyConvertedFrameDataToArray(colorSize, colorBuffer,
      ColorImageFormat::ColorImageFormat_Rgba);

    // �J���[�t���[�����J������
    colorFrame->Release();

    // �J���[�f�[�^���e�N�X�`���ɓ]������
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, colorWidth, colorHeight,
      GL_RGBA, GL_UNSIGNED_BYTE, colorBuffer);

    return true;
  }

  return false;
}

// �Z���T�̎��ʎq
IKinectSensor *KinectV2::sensor(NULL);
