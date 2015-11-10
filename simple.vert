#version 150 core
#extension GL_ARB_explicit_attrib_location : enable

// ����
uniform vec4 lamb;                                  // ��������
uniform vec4 ldiff;                                 // �g�U���ˌ�����
uniform vec4 lspec;                                 // ���ʔ��ˌ�����
uniform vec4 pl;                                    // �ʒu

// �ގ�
uniform vec4 kamb;                                  // �����̔��ˌW��
uniform vec4 kdiff;                                 // �g�U���ˌW��
uniform vec4 kspec;                                 // ���ʔ��ˌW��
uniform float kshi;                                 // �P���W��

// �ϊ��s��
uniform mat4 mw;                                    // ���_���W�n�ւ̕ϊ��s��
uniform mat4 mc;                                    // �N���b�s���O���W�n�ւ̕ϊ��s��
uniform mat4 mg;                                    // �@���x�N�g���̕ϊ��s��
uniform mat4 mt;                                    // �e�N�X�`�����W�̕ϊ��s��

// �e�N�X�`��
uniform sampler2D depth;

// ���_����
layout (location = 0) in vec4 ctex;                 // �J���[�̃e�N�X�`�����W
layout (location = 1) in vec4 pv;                   // ���_�̃J�������W

// ���X�^���C�U�ɑ��钸�_����
out vec4 idiff;                                     // �g�U���ˌ����x
out vec4 ispec;                                     // ���ʔ��ˌ����x
out vec2 texcoord;                                  // �e�N�X�`�����W

void main(void)
{
  // �@���x�N�g���̎Z�o
  vec4 nv = vec4(0.0, 0.0, 1.0, 0.0);

  // ���W�v�Z
  vec4 p = mw * pv;                                 // ���_���W�n�̒��_�̈ʒu
  vec4 q = pl;                                      // ���_���W�n�̌����̈ʒu
  vec3 v = normalize(p.xyz / p.w);                  // �����x�N�g��
  vec3 l = normalize((q * p.w - p * q.w).xyz);      // �����x�N�g��
  vec3 n = normalize((mg * nv).xyz);                // �@���x�N�g��
  vec3 h = normalize(l - v);                        // ���ԃx�N�g��

  // �A�e�v�Z
  idiff = max(dot(n, l), 0.0) * kdiff * ldiff + kamb * lamb;
  ispec = pow(max(dot(n, h), 0.0), kshi) * kspec * lspec;

  // �e�N�X�`�����W
  texcoord = ctex.st;

  // �N���b�s���O���W�n�ɂ�������W�l
  gl_Position = mc * pv;
}
