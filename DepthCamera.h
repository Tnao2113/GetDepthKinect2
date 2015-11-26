#pragma once

//
// �[�x�Z���T�̊��N���X
//

class DepthCamera
{
protected:

  // �f�v�X�J�����̃T�C�Y
  int depthWidth, depthHeight;

  // �J���[�J�����̃T�C�Y
  int colorWidth, colorHeight;

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
};
