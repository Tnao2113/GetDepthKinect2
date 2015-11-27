#pragma once

//
// �[�x�Z���T�̊��N���X
//

class DepthCamera
{
protected:

  // �f�v�X�J�����̃T�C�Y�Ɖ�f��
  int depthWidth, depthHeight, depthCount;

  // �f�v�X�f�[�^���i�[����e�N�X�`��
  GLuint depthTexture;

  // �f�v�X�f�[�^����ϊ������|�C���g�̃J�������W���i�[����e�N�X�`��
  GLuint pointTexture;

  // �J���[�J�����̃T�C�Y�Ɖ�f��
  int colorWidth, colorHeight, colorCount;

  // �J���[�f�[�^���i�[����e�N�X�`��
  GLuint colorTexture;

  // �f�v�X�f�[�^�̉�f�ɂ�����J���[�f�[�^�̃e�N�X�`�����W�l���i�[����o�b�t�@�I�u�W�F�N�g
  GLuint coordBuffer;

  // depthCount �� colorCount ���v�Z���ăe�N�X�`���ƃo�b�t�@�I�u�W�F�N�g���쐬����
  void makeTexture()
  {
    // �f�v�X�f�[�^�ƃJ���[�f�[�^�̉�f�������߂�
    depthCount = depthWidth * depthHeight;
    colorCount = colorWidth * colorHeight;

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

    // �J���[�f�[�^���i�[����e�N�X�`������������
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, colorWidth, colorHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // �f�v�X�f�[�^�̉�f�ʒu�̃J���[�̃e�N�X�`�����W���i�[����o�b�t�@�I�u�W�F�N�g����������
    glGenBuffers(1, &coordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, coordBuffer);
    glBufferData(GL_ARRAY_BUFFER, depthCount * 2 * sizeof (GLfloat), nullptr, GL_DYNAMIC_DRAW);
  }

public:

  // �R���X�g���N�^
  DepthCamera()
  {
  }
  DepthCamera(int depthWidth, int depthHeight, int colorWidth, int colorHeight)
    : depthWidth(depthWidth)
    , depthHeight(depthHeight)
    , colorWidth(colorWidth)
    , colorHeight(colorHeight)
  {
  }

  // �f�X�g���N�^
  virtual ~DepthCamera()
  {
    if (getActivated() > 0)
    {
      // �o�b�t�@�I�u�W�F�N�g���폜����
      glDeleteBuffers(1, &coordBuffer);

      // �e�N�X�`�����폜����
      glDeleteTextures(1, &depthTexture);
      glDeleteTextures(1, &colorTexture);
      glDeleteTextures(1, &pointTexture);
    }
  }

  // �f�v�X�f�[�^���擾����
  GLuint getDepth() const
  {
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    return depthTexture;
  }

  // �J�������W���擾����
  GLuint getPoint() const
  {
    glBindTexture(GL_TEXTURE_2D, pointTexture);
    return pointTexture;
  }

  // �J���[�f�[�^���擾����
  GLuint getColor() const
  {
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    return colorTexture;
  }

  // �f�v�X�J�����̃T�C�Y�𓾂�
  void getDepthResolution(int *width, int *height) const
  {
    *width = depthWidth;
    *height = depthHeight;
  }

  // �J���[�J�����̃T�C�Y�𓾂�
  void getColorResolution(int *width, int *height) const
  {
    *width = colorWidth;
    *height = colorHeight;
  }

  // �J���[�f�[�^�̃e�N�X�`�����W�l���i�[����o�b�t�@�I�u�W�F�N�g�𓾂�
  GLuint getCoordBuffer() const
  {
    return coordBuffer;
  }

  // �g�p���Ă���Z���T�[�̐��𒲂ׂ�
  virtual int getActivated() const = 0;
};
