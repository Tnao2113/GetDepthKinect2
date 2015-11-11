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

    // �J���[�f�[�^�̓ǂݍ��ݐݒ�
    assert(sensor->get_ColorFrameSource(&colorSource) == S_OK);
    assert(colorSource->OpenReader(&colorReader) == S_OK);
    assert(colorSource->get_FrameDescription(&colorDescription) == S_OK);

    // �J���[�f�[�^�̃T�C�Y�𓾂�
    colorDescription->get_Width(&color_w);
    colorDescription->get_Height(&color_h);

    // �J���[�f�[�^�ϊ��p�̈ꎞ�o�b�t�@���m�ۂ���
    color_size = color_w * color_h * 4;
    color_buffer = new BYTE[color_size];

    // �J���[���i�[����e�N�X�`������������
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, color_w, color_h, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // �f�v�X�f�[�^�̓ǂݍ��ݐݒ�
    assert(sensor->get_DepthFrameSource(&depthSource) == S_OK);
    assert(depthSource->OpenReader(&depthReader) == S_OK);
    assert(depthSource->get_FrameDescription(&depthDescription) == S_OK);

    // �f�v�X�f�[�^�̃T�C�Y�𓾂�
    depthDescription->get_Width(&depth_w);
    depthDescription->get_Height(&depth_h);
    depth_count = depth_w * depth_h;

    // �f�v�X���i�[����e�N�X�`������������
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, depth_w, depth_h, 0, GL_RED, GL_UNSIGNED_SHORT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // �`�悷��O�p�`�̒��_�̑��������߂�
    vertexCount = ((depth_w - 1) * (depth_h - 1) * 3 * 2);
      
      // �`��Ɏg�����b�V���f�[�^�̒��_�z��I�u�W�F�N�g����������
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // �`��Ɏg�����b�V���f�[�^�̒��_�o�b�t�@�I�u�W�F�N�g����������
    glGenBuffers(sizeof vbo / sizeof vbo[0], vbo);

    // �J���[�}�b�v���f�v�X�}�b�v�̉�f�P�ʂɃT���v�����O���邽�߂̃o�b�t�@�I�u�W�F�N�g
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, depth_count * sizeof(ColorSpacePoint), NULL, GL_DYNAMIC_DRAW);

    // �C���f�b�N�X�� 0 �� varying �ϐ��Ɋ��蓖�Ă�
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // �f�v�X�}�b�v���狁�߂��J�������W�n�̈ʒu
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, depth_count * sizeof(CameraSpacePoint), NULL, GL_DYNAMIC_DRAW);

    // �C���f�b�N�X�� 1 �� varying �ϐ��Ɋ��蓖�Ă�
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // �C���f�b�N�X�p�̃o�b�t�@�I�u�W�F�N�g
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexCount * sizeof(GLuint), NULL, GL_STATIC_DRAW);

    // �C���f�b�N�X�����߂ăo�b�t�@�I�u�W�F�N�g�ɓ]������
    GLuint *index(static_cast<GLuint *>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY)));
    for (int j = 0; j < depth_h - 1; ++j)
    {
      for (int i = 0; i < depth_w - 1; ++i)
      {
        index[0] = depth_w * j + i;
        index[1] = index[5] = index[0] + 1;
        index[2] = index[4] = index[0] + depth_w;
        index[3] = index[2] + 1;
        index += 6;
      }
    }
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
  }
}

// �f�X�g���N�^
KinectV2::~KinectV2()
{
  if (sensor != NULL)
  {
    // �f�[�^�ϊ��p�̈ꎞ�o�b�t�@���J������
    delete[] color_buffer;

    // ���_�z��I�u�W�F�N�g���폜����
    glDeleteVertexArrays(1, &vao);

    // ���_�o�b�t�@�I�u�W�F�N�g���폜����
    glDeleteBuffers(3, vbo);

    // �Z���T�������[�X����
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
    colorFrame->CopyConvertedFrameDataToArray(color_size, color_buffer,
      ColorImageFormat::ColorImageFormat_Bgra);

    // �J���[�f�[�^���e�N�X�`���ɓ]������
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, color_w, color_h,
      GL_RGBA, GL_UNSIGNED_BYTE, color_buffer);

    return true;
  }

  return false;
}

// �f�v�X�f�[�^���擾����
bool KinectV2::getDepth() const
{
  // �f�v�X�̃e�N�X�`�����w�肷��
  glBindTexture(GL_TEXTURE_2D, depthTexture);

  // ���̃J���[�̃t���[���f�[�^���������Ă����
  IDepthFrame *depthFrame;
  if (depthReader->AcquireLatestFrame(&depthFrame) == S_OK)
  {
    // �f�v�X�f�[�^�̃T�C�Y�Ɗi�[�ꏊ�𓾂�
    UINT depth_size;
    UINT16 *depth_buffer;
    depthFrame->AccessUnderlyingBuffer(&depth_size, &depth_buffer);

    // �f�v�X�f�[�^���e�N�X�`���ɓ]������
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, depth_w, depth_h,
      GL_RED, GL_UNSIGNED_SHORT, depth_buffer);

    // �J���[�̃e�N�X�`�����W���쐬���ăo�b�t�@�I�u�W�F�N�g�ɓ]������
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    ColorSpacePoint *const texCoord(static_cast<ColorSpacePoint *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
    coordinateMapper->MapDepthFrameToColorSpace(depth_count, depth_buffer, depth_count, texCoord);
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // �J�������W�n�̒��_�ʒu�����߂ăo�b�t�@�I�u�W�F�N�g�ɓ]������
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    CameraSpacePoint *const position(static_cast<CameraSpacePoint *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)));
    coordinateMapper->MapDepthFrameToCameraSpace(depth_count, depth_buffer, depth_count, position);
    glUnmapBuffer(GL_ARRAY_BUFFER);

    return true;
  }

  return false;
}

// �Z���T�̎��ʎq
IKinectSensor *KinectV2::sensor(NULL);
