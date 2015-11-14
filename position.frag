#version 150 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable

#define MILLIMETER 0.001
#define DEPTH_SCALE (-65535.0 * MILLIMETER)
#define DEPTH_MAXIMUM (-10.0)

// �X�P�[��
const vec2 scale = vec2(
  1.546592,
  1.222434
);

// �e�N�X�`��
layout (location = 0) uniform sampler2D depth;

// �e�N�X�`�����W
in vec2 texcoord;

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec3 position;

// �[�x�l���X�P�[�����O����
float s(in float z)
{
  return z == 0.0 ? DEPTH_MAXIMUM : z * DEPTH_SCALE;
}

void main(void)
{
  // �[�x�����o��
  float z = s(texture(depth, texcoord).r);

  // �[�x������W�l�����߂�
  position = vec3((texcoord - 0.5) * scale * z, z);
}
