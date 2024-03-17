#pragma once
// ====== �C���N���[�h�� ======
#include <windows.h>

// ====== �萔��` ======
// �E�B���h�E�̃^�C�g��
#define WINDOW_NAME "DXForge"

// �E�B���h�E�̕�
const int WINDOW_WIDTH = 1280;
// �E�B���h�E�̍���
const int WINDOW_HEIGHT = 720;
// �E�B���h�E�̃A�X�y�N�g��
const float ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

// ====== �v���g�^�C�v�錾 ======
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
void CreateMainWindow(int w, int h, bool fullScreen);
HWND GetWindowHandle();
