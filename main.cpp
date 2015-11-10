//
// Kinect (v1) �̃f�v�X�}�b�v�擾
//

// �W�����C�u����
#include <Windows.h>
#include <memory>

// �E�B���h�E�֘A�̏���
#include "Window.h"

// �Z���T�֘A�̏���
#include "Sensor.h"

//
// ���C���v���O����
//
int main()
{
  // GLFW ������������
  if (glfwInit() == GL_FALSE)
  {
    // GLFW �̏������Ɏ��s����
    MessageBox(NULL, TEXT("GLFW �̏������Ɏ��s���܂����B"), TEXT("���܂�̂�"), MB_OK);
    return EXIT_FAILURE;
  }

  // �v���O�����I�����ɂ� GLFW ���I������
  atexit(glfwTerminate);

  // OpenGL Version 3.2 Core Profile ��I������
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // �E�B���h�E���J��
  Window window(640, 480, "Depth Map Viewer");
  if (!window.get())
  {
    // �E�B���h�E���쐬�ł��Ȃ�����
    MessageBox(NULL, TEXT("GLFW �̃E�B���h�E���J���܂���ł����B"), TEXT("���܂�̂�"), MB_OK);
    return EXIT_FAILURE;
  }

  // �[�x�Z���T��L���ɂ���
  Sensor sensor;
  if (sensor.getActivated() == 0)
  {
    // �Z���T���g���Ȃ�����
    MessageBox(NULL, TEXT("�[�x�Z���T��L���ɂł��܂���ł����B"), TEXT("���܂�̂�"), MB_OK);
    return EXIT_FAILURE;
  }

  // ��`�f�[�^
  const std::unique_ptr<const GgTriangles> rect(ggRectangle());

  // �V�F�[�_
  GgSimpleShader simple("simple.vert", "simple.frag");

  // uniform �ϐ��̏ꏊ
  const GLint colorLoc(glGetUniformLocation(simple.get(), "color"));
  const GLint depthLoc(glGetUniformLocation(simple.get(), "depth"));

  // �w�i�F��ݒ肷��
  glClearColor(background[0], background[1], background[2], background[3]);

  // �B�ʏ���������L���ɂ���
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // �E�B���h�E���J���Ă���Ԃ���Ԃ��`�悷��
  while (!window.shouldClose())
  {
    // ��ʏ���
    window.clear();

    // �V�F�[�_�v���O�����̎g�p�J�n
    simple.use();
    simple.loadMatrix(window.getMp(), window.getMw());
    simple.setLight(light);
    simple.setMaterial(material);

    // �e�N�X�`��
    glUniform1i(colorLoc, 0);
    glActiveTexture(GL_TEXTURE0);
    sensor.getColor();
    glUniform1i(depthLoc, 1);
    glActiveTexture(GL_TEXTURE1);
    sensor.getDepth();

    // �}�`�`��
    sensor.draw();

    // �o�b�t�@�����ւ���
    window.swapBuffers();
  }
}
